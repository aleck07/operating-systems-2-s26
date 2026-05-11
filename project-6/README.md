# Project 6: Very Very Simple File System (VVSFS)

A simulated file system implemented in C. The file system is stored in a flat
disk image file. Read and write operations go through a block abstraction layer.

## Building

* `make` to build. An executable called `testfs` will be produced.
* `make test` to build and run the test suite.
* `make clean` to remove object files.
* `make pristine` to remove all build products, including `testfs` and `libvvsfs.a`.

## Files

* `image.c`: Open and close the disk image file
* `image.h`: Header for image functions
* `block.c`: Read and write 4096-byte blocks within the image
* `block.h`: Header for block functions and the `BLOCK_SIZE` constant
* `testfs.c`: Test suite using `ctest.h`

## Data

The disk image is a flat file divided into 4096-byte blocks. The global
`image_fd` holds the file descriptor for the open image.

Block layout:

* Block 0: Superblock
* Block 1: Inode map
* Block 2: Block map
* Blocks 3-6: Inode data
* Blocks 7+: File data

Block buffers are `unsigned char[BLOCK_SIZE]` arrays passed into `bread` and
`bwrite`.

## Functions

* `image_open()`: Open or create the disk image file, with truncate flag
* `image_close()`: Close the disk image file
* `bread()`: Seek to a block's offset and read it into a buffer
* `bwrite()`: Seek to a block's offset and write a buffer to it

## Notes

* Block N occupies bytes `N * 4096` through `(N+1) * 4096 - 1` in the image file.
* `bread()` zero-fills any bytes not returned by `read()` (e.g. reading past end of file).
