# Filesystem - Lab checksum
This xv6 patch is a modification of the "09K - Filesystem (Stats)" lab.

The goal was to add a level of data protection from corruption.
The first thing to do is to understand how the file system is laid out on disk. It is actually a
pretty simple layout, as described in the upper part of fs.c there is a superblock,
then some inodes, a single bitmap block in use and some blocks of data.

  +------------+--------+--------+------+
  | superblock | inodes | bitmap | data |
  +------------+--------+--------+------+
  
Its structure is defined in the fs.h file

struct superblock {
  uint size;         // Size of file system image (blocks)
  uint nblocks;      // Number of data blocks
  uint ninodes;      // Number of inodes.
  uint nlog;         // Number of log blocks
  uint logstart;     // Block number of first log block
  uint inodestart;   // Block number of first inode block
  uint bmapstart;    // Block number of first free map block
};

Each inode is represented in the following way

struct dinode {
  short type;           	// File type (regular file or directory)
  short major;          	// Major/Minor device number (T_DEV only) --> we know which disc to read!
  short minor;
  short nlink;          	// Number of links to inode in file system --> we know how many directories the file is linked to
  uint size;            	// Size of file (bytes)
  uint addrs[NDIRECT+1];   	/* Data block addresses --> where the first NDIRECT addresses are direct pointers to the first NDIRECT blocks of the file (if it is so large). The last block is reserved 					   for larger files that need an indirect block therefore, the latter element of the array is the disk address of the indirect block. The indirect block itself, of course, 					   can contain many more block pointers for these larger files.*/
};

struct dirent {
  ushort inum;
  char name[DIRSIZ];
};

My first change was to keep the inode structure intact, to use the slots for direct pointers as a pair (checksum, pointer).
To do this, each direct pointer slot is composed of 4 bytes: 1 byte I have reserved for the checksum and 3 bytes to the pointer.
Obviously, this limits the addressing space of the file system (to 2 ^ 24) to which it can refer.

Now that I have prepared the necessary space for information, I will proceed with its calculation.
The checksum calculates it on each block, then as XOR of all the bytes of the block and I store that value in the pointer to the block as
I described above. This operation is repeated every time a "particular" block is written, so that the filesystem keeps
checksums updated. My version has room to return exactly one byte of checksum information to the file, so it was
XOR must all existing checksums on blocks of the file and return that value in the checksum field of the statistics structure.
With reference to the code, the checksum in the fs.c file is initially calculated in this way in the
stati(struct inode * ip, struct stat * st) function:

	if (ip->type == T_CHECKED) {						// IF the file is of the new type
	uint checksum = 0;						        // initialize my variable
	for(int i = 0; i < NDIRECT; i++)					// for each direct block
	{
	  checksum ^= (ip->addrs[i] >> 24) & 0xff;				// ^= bitwise XOR assignment
	}
	if (ip->addrs[NDIRECT] != 0) {						// verification of the presence of indirect blocks
	  struct buf *indirect_bp = bread(ip->dev, ip->addrs[NINDIRECT]);	/* the bread function reads a corresponding block and marks it as B_BUSY and returns the buffer, if it does not find it 										   (because for example the block is not cached in memory), allocates it of the defined size and then reads*/
	  uint *addr = (uint*)indirect_bp->data;				// 
	  for(int i = 0; i < NINDIRECT; i++)					// for each indirect block
	  {
		checksum ^= ((*addr) >> 24) & 0xff;				/* as before, right shift bitwise of 24 (the 3 bytes of the pointer), then bitwise AND with FF in hexadecimal or 255 											   integer, to consider the value of the last 8 bits (1 byte of our checksum) */
                addr++;								// increment for next address
	  }
	  brelse(indirect_bp);							// release the previous block and mark it as unoccupied (reverse op. of bread)
	}
	st->checksum = (uchar) checksum;					// update my data structure with the new checksum value
	} 

At each read operation of a block with a checksum, the filesystem reads in the block, calculates its checksum and compares it with
the one stored (the one I inserted in the pointer). If the two values match, it returns the correct information, otherwise the call
read () returns an error (-1). Since the filesystem does not create block replicas, it is not necessary to restore but only report it
to the user and not to return corrupt information, these operations are shown in detail below.
Particular attention must be paid to large files because they will have an indirect pointer assigned and therefore also the block
indirect will be full of direct pointers, which in turn must contain the checksum described above.

The original system initially only supports the existing pointer-based structure, the following changes were required to add the new checksum-based structure.
The first was to add a new type of file in stat.h, called T_CHECKED with the numeric value of 4.
So for normal files the original structure is used (without checksum), while for the management of new files with extra protection
(T_CHECKED) must use the new checksum-based structure. 

	#define T_DIR  1   	 // Directory
	#define T_FILE 2   	 // File
	#define T_DEV  3   	 // Device
	#define T_CHECKED 4  	 // Checksum			<-- definition of the new file type

	struct stat {
	  short type;  		// Type of file
	  int dev;     		// File system's disk device
	  uint ino;    		// Inode number
	  short nlink; 		// Number of links to file
	  uint size;   		// Size of file in bytes
	  uchar checksum;	// Checker the file		<-- new parameter
	};

For the creation of a file based on this new structure, it was necessary to modify the interface, adding an O_CHECKED flag
(with value 0x400) at the open () system call, which normally creates the files.
When this flag is present, the file system should create a checksum-based file, with all the changes expected as described above.
The new flag has been added to the fcntl.h file, as shown below:

	#define O_RDONLY  0x000
	#define O_WRONLY  0x001
	#define O_RDWR    0x002
	#define O_CREATE  0x200
	#define O_CHECKED 0x400 			// <-- new flag 

Creating the new file type in the open () function of the sysfile.c file, as follows:

    if (omode & O_CHECKED) {				// flag present
      ip = create(path, T_CHECKED, 0, 0);		// create the file with the new input flag
    }

In addition to the open () system call, it was also necessary to modify the read (), write () and bmap() system calls in the fs.c file, 
which was able to manage the new file types and their checksum-based structure.

	// READI -->

    int isFind = 0; 										// To check whether checksum is find in direct part
    if (ip->type == T_CHECKED) {
      int i;
      uint addr = 0;
      for( i = 0; i < NDIRECT; i++)								// Find the address in the direct data block
      {
        if((ip->addrs[i] & 0x00ffffff)== block){						// consider only the 3 byte of pointer that coincide, unlike before
          isFind = 1;
          addr = ip->addrs[i];									// assign the address
          cprintf("addr: %d, block size: %d\n", ip->addrs[i], block);
          break;
        }

      }
      if (isFind == 0 && ip->addrs[NDIRECT]!=0 ) {
        struct buf *buf = bread(ip->dev, ip->addrs[NDIRECT]);
        uint* ptr = (uint*)buf->data;
        for(int i = 0; i < NINDIRECT; i++)							// Find the address in the indirect data block
        {
          if(((*ptr) & 0x00ffffff) == block){							// same condition
            addr = *ptr;									// assign the address
          }
        }
        brelse(buf);       
      } 
      // Il ricalcolo del checksum è identico a quello visto in precedenza nella funzione stati()
      int checksum = 0;
      char* data = (char*)bp->data;
      for(int i = 0; i < BSIZE; i++)
      {
          checksum = checksum ^ (uint)* data;
          data++;
      }
      cprintf("addr is %d\n", addr);
      uint old = (addr >> 24);
      if (checksum == old) {
        return 1;										// return OK
      }else{
        return -1;										// return error
        cprintf("Checksum is unmatch in Readi()\n");
      }
    }
	
	// WRITEI -->
	
    if (ip->type == T_CHECKED) {
      uint checksum = 0;
      char* data = (char*)bp->data;
      for(int i = 0; i < BSIZE; i++)
      {
        checksum = checksum ^ (uint)*data;
        data++;
      }

      int blockNum = -1;
      for(int i = 0; i < NDIRECT; i++)								// Check direct part
      {
        if((ip->addrs[i] & 0x00ffffff) == block){						// same check of the readi
          blockNum = i;
          break;
        }  
      }
      if (blockNum > -1) {
        ip->addrs[blockNum] = block + (checksum << 24);
      }else{
        struct buf *indirect_buf = bread(ip->dev,ip->addrs[NDIRECT]);
        uint *ptr = (uint*)indirect_buf->data;
        for(int i = 0; i < NINDIRECT; i++)							//Check indirect part
        {
            if(((*ptr) & 0x00ffffff) == block){							// like before 
            *ptr = block + (checksum << 24);							// bit of the address of the block plus those of checksum, right shift by 24
												// for leave the space at the 3 byte, note the explanation before
            break;
          }
          ptr++;
        }
        bwrite(indirect_buf);									// write a block and wait that I/O done, unless the flag B_ASYNC isn't set 
        brelse(indirect_buf);									// release the buffer (reverse of bread)
      }
    }

	// BMAP -->

    if(ip->type == T_CHECKED) 									// for checksum file
      addr = 0x00ffffff & addr;									// the address in this case is only 3 byte !

Finally, I modified the stat () system call to return the actual disk address information to the inode (in the version
of the laboratory returns only some information).
To test the new version, you need to call filestat, which can be called like this: filestat pathname. If done in this way, the filestat program should print all information about a file, including its type, size and this new checksum value. In the event that the file does not already exist, it is created (with static size, but can be modify with the #define in filestat.c) according to the new structure, then returns the information. So that shows that the checksum-based file system works.


