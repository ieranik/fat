#Overview

This repository contains implementation of the FAT file system for a 1 MB disk (virtual disk). The disk is divided into 4096 blocks each of size 256 bytes (4096*256 bytes = 1 MB). Each FAT entry is 4 bytes. As FAT contains one entry for each block of the disk, there will be 4096 entries in the FAT each of size 4 bytes. Thus FAT uses 16 KB (= 4096*4 bytes) space in the disk. The first 64 (=16 KB/256bytes) blocks (Block#0 to Block#63) are reserved to allocate the FAT array. Block#64 is also reserved for storing any system variable or data there if needed. The first block of the root directory is stored in Block#65. So the file number of the root directory is 65 in this system. Block#66 to Block#4095 can be used to store files and directories. The following figure illustrates the layout of the disk as discussed above.

<p align="center">
<img src="https://github.com/ieranik/rcm/images/fat1.png">

#Code Description

The 4096X256 char array named disk is the only form of storage available. The whole FAT array is encoded in the first 64 rows of disk. Row 64 if the disk is used to create and store any variable that must survive across method calls. For example, the locality heuristic of FAT system is next fit. To implement next fit, we need to remember the number of the last block that was allocated. So we use the first 4 bytes (actually 12 bits are sufficient) of Block#64 to store the number of the last allocated block.

When a file is created, initially it contains the character ‘a’ in all its data bytes. Later other bytes can be written in the data bytes of the file through the write file command.

The layout of the first block of a directory file is shown in the following figure. In the first 32 bytes, we store the parent directory number and any auxiliary information. Then 7 directory entries follow which store information about the child files and sub-directories located in the directory. Each directory entry is of size 32 bytes and contains the (file name, file number) pair and the metadata (creation time and size) of the corresponding child file or sub-directory. Note that, if a directory contains more than 7 child files and/or directories, new blocks need to be allocated each of which will store 8 (=256/32) directory entries. We use linear search to find, delete and insert directory entries. We don’t store size of directories.

<p align="center">
<img src="https://github.com/ieranik/rcm/images/fat2.png">




