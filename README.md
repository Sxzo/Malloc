# A Basic Implementation of libc's Malloc, Calloc, and Realloc

## Basic Allocator Design:

**1. Block Splitting:**

Blocks of memory on the heap are split upon allocation to leave free space for future allocations. If the open chunk of space has a byte size <= the size of a metadata tag, its disregarded.

**2. Memory Coalescing:**

Neighboring blocks of free memory seperated by metadata tags are coalesced to form a combined larger block. This optimization not only frees the space of the uneeded metadata tag, but prevents memory fragmentation after numerous allocations.

**3. Free Lists:**

Instead of traversing across all blocks of memory, we can greatly optimize finding available storage by creating a linked list that contains only free blocks of memory. Without free lists, attempting to allocate memory on a highly occupied heap would be substantially slower.
## Testing:

This implementation of Malloc is tested by sideloading it into programs that are designed specifically for stress testing alongside actual programs like ``gif.c``.

This project was completed for CS340 (Systems) during spring of 2023. It allows insight into the way that Malloc works and the way it's implemented by libc in the backend. Alongside this, this project provides an invaluable understanding of the true importance of linked lists in systems. 



