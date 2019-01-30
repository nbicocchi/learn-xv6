## Operating Systems Course - DIEF UNIMORE ##

In this exercise, we will modify xv6's filesystem to increase the maximum file size.

### Inode Improvements
xv6 uses an inode based filesystem where each inode contains 12 direct data block pointers and 1 single indirect block pointer, where a block "pointer" is just a 4-byte disk block number (indexed from the first sector on the drive). Because a block is 512 bytes large, the indirect block can hold an additional 128 block numbers, resulting in a maximum file size of 140 blocks, which equals a puny 70KB.

For this lab you'll modify xv6 to change the inode structure so that it contains:

- 10 direct block numbers - 3 single indirect block numbers. Using this scheme, we can address a total of 10 + 128 + 128 + 128 = 394 blocks, or just over 200K.

### Hints
You will need to modify, at minimum, the "file.h", "fs.h", and "fs.c" files. Most of your work will take place in the bmap function (found in "fs.c"), which takes a pointer to an in-memory inode and a logical block number (i.e., an offset from the start of the file), and returns the corresponding disk block number. Note that bmap is called when reading and writing a file, so if it is given a logical block number beyond the current end of file (but within the maximum file size), bmap will allocate the necessary disk block(s) and update the inode structure(s).

struct dinode, in "fs.h", defines the on-disk inode structure, and struct inode, in "file.h", represents an in-memory inode. The addrs component of these structures is particularly important; its first NDIRECT entries are disk block numbers for data blocks (when allocated). You'll need to change / add to the constants NDIRECT, NINDIRECT, and MAXFILE (in "fs.h"), which are specified in numbers of blocks.

bmap makes use of balloc to allocate new (zeroed) disk blocks, bread to read the contents of a disk block, and brelse to release a lock on a block. You need to ensure that all blocks you read (using bread) are released. bread reads disk blocks into struct buf structures (defined in "buf.h"), and raw data can be accessed via the structure's data component. Finally, log_write must also be called with a pointer to each struct buf that contains modified data.

Read chapter 6 of the xv6 book, focusing on the "Block allocator" and "Inodes" sections, for useful information. You should also read through the original bmap first, to make sure you understand how it works.

### Testing
To test your program, you'll run the program bigtest (when booted into xv6), which will try to write to, then read from as big of a file as it can. Without any changes, it should report that it can write 140 sectors. After your changes, it should report that it can write 394 sectors.

Good work!

