## Operating Systems Course - DIEF UNIMORE ##

In this exercise, was modified xv6's filesystem to increase the maximum file size.

### Inode Improvements
xv6 uses an inode based filesystem where each inode contains 12 direct data block pointers and 1 single indirect block pointer, where a block "pointer" is just a 4-byte disk block number (indexed from the first sector on the drive). Because a block is 512 bytes large, the indirect block can hold an additional 128 block numbers, resulting in a maximum file size of 140 blocks, which equals 70KB.

In this lab we modified xv6 to change the inode structure so that it contains:

For single indirection: 13-N direct blocks, N indirect blocks. Using this scheme, we can address a total of (13-N) + Nx128 blocks. With N=1, we can address 140 blocks (70K). With N=3, we can address 394 blocks (197K).

For double indirection: 13-N direct blocks, N double indirect blocks. Each double indirect block uses only NINDIRECTIB blocks for indirection. Using this scheme, we can address a total of 13-N + Nx(128-NINDIRECTIB) + NXNINDIRECTIBX128 blocks. With N=1 and NINDIRECTIB=4 we can adress up to 648 blocks (324K).

For triple indirection: similar to double indirection but with an additional level included.

When crucial parameters such as NDIRECT, NINDIRECT or NINDIRECTIB are modified it might be needed to modify FFSIZE in param.h accordingly. Otherwise the available blocks could not be enough.

### Hints
For modifying file size it was necessary to modify the "file.h", "fs.h", "fs.c" and when necessary param.h files. Most of work was done in the bmap (found in "fs.c") and iappend function (found mkfs.c). The former takes a pointer to an in-memory inode and a logical block number (i.e., an offset from the start of the file), and returns the corresponding disk block number. Note that bmap is called when reading and writing a file, so if it is given a logical block number beyond the current end of file (but within the maximum file size), bmap will allocate the necessary disk block(s) and update the inode structure(s). The latter takes i-number, a buffer/struct dirent and its size to mount file system and insert binaries files at time of boot. Note that this (iappend) must have the same structures of (bmap) otherwise won't be possible writing or reading correctly into filesystem.

struct dinode, in "fs.h", defines the on-disk inode structure, and struct inode, in "file.h", represents an in-memory inode. The addrs component of these structures is particularly important; its first NDIRECT entries are disk block numbers for data blocks (when allocated). You'll need to change / add to the constants NDIRECT, NINDIRECT, and MAXFILE (in "fs.h"), which are specified in numbers of blocks.

bmap makes use of balloc to allocate new (zeroed) disk blocks, bread to read the contents of a disk block, and brelse to release a lock on a block. You need to ensure that all blocks you read (using bread) are released. bread reads disk blocks into struct buf structures (defined in "buf.h"), and raw data can be accessed via the structure's data component. Finally, log_write must also be called with a pointer to each struct buf that contains modified data.

iappend makes use of freeblock to tag sector as allocated, rsect and wsect for reading and writing sectors.

### Testing
To test the modification, run the program bigtest (when booted into xv6), which will try to write to and read from the biggest file possible (until write() fails). Without any changes, it should report 140 sectors.
