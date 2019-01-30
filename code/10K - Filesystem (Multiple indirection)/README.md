## Operating Systems Course - DIEF UNIMORE ##

In this exercise, was modified xv6's filesystem to increase the maximum file size.

### Inode Improvements
xv6 uses an inode based filesystem where each inode contains 12 direct data block pointers and 1 single indirect block pointer, where a block "pointer" is just a 4-byte disk block number (indexed from the first sector on the drive). Because a block is 512 bytes large, the indirect block can hold an additional 128 block numbers, resulting in a maximum file size of 140 blocks, which equals 70KB.

For this lab was modified xv6 to change the inode structure so that it contains:

For double indirection: 12 direct block numbers - 1 double indirect block number. Using this scheme, we can address a total of 12 + 127 + 128 = 267 blocks, or just over 136K. It is possible to increase file size modifying only voice NINDIRECTIB in fs.h for this the name of Dynamic-Double-Indirect. 

For triple indirection: 12 direct block numbers - 1 triple indirect block number. Using this scheme, we can address a total of 12 + 127 + 127 + 128 = 394 blocks, or just over 200K. It is possible to increase file size modifying only voice NINDIRECTIB in fs.h for this the name of Dynamic-Triple-Indirect.

When NINDIRECT is modified there is need to modify FFSIZE in param.h accordingly, otherwise block numbers are not sufficient to wrap all data.

### Hints
For modifying file size it was necessary to modify the "file.h", "fs.h", "fs.c" and when necessary param.h files. Most of work was done in the bmap (found in "fs.c") and iappend function (found mkfs.c). The former takes a pointer to an in-memory inode and a logical block number (i.e., an offset from the start of the file), and returns the corresponding disk block number. Note that bmap is called when reading and writing a file, so if it is given a logical block number beyond the current end of file (but within the maximum file size), bmap will allocate the necessary disk block(s) and update the inode structure(s). The latter takes i-number, a buffer/struct dirent and its size to mount file system and insert binaries files at time of boot. Note that this (iappend) must have the same structures of (bmap) otherwise won't be possible writing or reading correctly into filesystem.

struct dinode, in "fs.h", defines the on-disk inode structure, and struct inode, in "file.h", represents an in-memory inode. The addrs component of these structures is particularly important; its first NDIRECT entries are disk block numbers for data blocks (when allocated). You'll need to change / add to the constants NDIRECT, NINDIRECT, and MAXFILE (in "fs.h"), which are specified in numbers of blocks.

bmap makes use of balloc to allocate new (zeroed) disk blocks, bread to read the contents of a disk block, and brelse to release a lock on a block. You need to ensure that all blocks you read (using bread) are released. bread reads disk blocks into struct buf structures (defined in "buf.h"), and raw data can be accessed via the structure's data component. Finally, log_write must also be called with a pointer to each struct buf that contains modified data.

iappend makes use of freeblock to tag sector as allocated, rsect and wsect for reading and writing sectors.

### Testing
To test program, you'll run the program bigtest (when booted into xv6), which will try to write to, then read from as big of a file as it can. Without any changes, it should report that it can write 140 sectors. After changes, it should report:

For double indirection: that it can write 267 sectors up to a maximum of 16.396 sectors or 8 MB.

For triple indirection: that it can write 394 sectors up to a maximum of 2.097.164 sectors or 1 GB.

