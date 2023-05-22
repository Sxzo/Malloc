/**
 * Malloc
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// sbrk increments the heap size. For example, sbrk(100) increases the heap by 100 bytes.

typedef struct _metadata_t {
  unsigned int size;
  unsigned char isUsed; // 0 if the block is free; 1 if its used
  struct _metadata_t* next;
  struct _metadata_t* prev;
} metadata_t;

void* startOfHeap = NULL;
metadata_t* head_ = NULL;
metadata_t* tail_ = NULL;

void printFreeList() {
  if (head_ == NULL) {
    printf("\n Free List is currently empty. \n");
    return;
  }
  metadata_t* temp = head_;
  int i = 1;
  while (temp != NULL) {
    printf("\n Free slot %i | Size: %i | isUsed: %i | Address: %p \n", i, temp->size, temp->isUsed, (void*)temp + sizeof(metadata_t));
    i++;
    temp = temp->next;
  }
}

void freeListRemove(metadata_t* to_remove) {
  if (head_ == NULL) return;
  if (head_ == tail_) {
    head_ = NULL;
    tail_ = NULL;
    return;
  }
  if (to_remove == head_) {
    head_->next->prev = NULL;
    head_ = head_->next;
    return;
  } else if (to_remove == tail_) {
    tail_->prev->next = NULL;
    tail_ = tail_->prev;
  } else {
    to_remove->prev->next = to_remove->next;
    to_remove->next->prev = to_remove->prev;
    to_remove->next = NULL;
    to_remove->prev = NULL;
  }
}

void freeListInsert(metadata_t* to_insert) { // Potential collapsing issue when coalescing the entire block. 
  
  // Coalescing check:
  metadata_t* temp = head_;
  while (temp != NULL) {
    if ((void*)to_insert + sizeof(metadata_t) + to_insert->size == (void*)temp) { // temp comes directly after to_insert
      to_insert->size += temp->size + sizeof(metadata_t);
      to_insert->isUsed = 0;
      temp->size = 0;
      freeListRemove(temp);
      freeListInsert(to_insert);
      return;
    }
    if ((void*)to_insert - temp->size - sizeof(metadata_t) == (void*)temp) { // temp comes before to_insert
      temp->size += to_insert->size + sizeof(metadata_t);
      return;
    } 
    temp = temp->next;
  }
  
  
  if (head_ == NULL) { // Empty with 0 elements.
    to_insert->next = NULL;
    to_insert->prev = NULL;
    head_ = to_insert;
    tail_ = to_insert; 
  } else if (head_ == tail_) { // List with 1 element.
    to_insert->prev = NULL;
    to_insert->next = head_;
    head_->prev = to_insert; 
    head_ = to_insert;
    tail_ = head_->next;
  } else { // List with 1+ elements. 
    to_insert->next = head_;
    to_insert->prev = NULL;
    head_->prev = to_insert;
    head_ = to_insert; 
  }
}

// ----------------------------------------------------------------------------

/**
 * Allocate space for array in memory
 *
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 *
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size) {
  // implement calloc:
  void* ptr = malloc(num * size);
  memset(ptr, 0, num*size);

  return ptr;
}



/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
void *malloc(size_t size) {
  //printf("Inside: malloc(%lu):\n", size);
  if (startOfHeap == NULL) {
    startOfHeap = sbrk(0);
  }

  // metadata_t *curMeta = startOfHeap;
  // void *endOfHeap = sbrk(0);
  // printf("-- Start of Heap (%p) --\n", startOfHeap);
  // while ((void *)curMeta < endOfHeap) {   // While we're before the end of the heap...
  //   printf("metadata for memory %p: (%p, size=%d, isUsed=%d)\n", (void *)curMeta + sizeof(metadata_t), curMeta, curMeta->size, curMeta->isUsed);
  //   curMeta = (void *)curMeta + curMeta->size + sizeof(metadata_t);
  // }
  // printf("-- End of Heap (%p) --\n\n", endOfHeap);

  // Reusing freed data:
  metadata_t* temp = head_; 
  // printFreeList();
  while (temp != NULL) {
    if (temp->size >= size) {
      if (temp->size - size <= sizeof(metadata_t)) { // If excess space is unsubstantial:
        
        temp->isUsed = 1;
        void* ptr = (void*)temp + sizeof(metadata_t);
        freeListRemove(temp);
        return ptr;

      } else { // If there's a valid amount of excess space:
        
        metadata_t* new_meta = (void*)temp + sizeof(metadata_t) + size;
        new_meta->isUsed = 0;
        new_meta->size = temp->size - size - sizeof(metadata_t);
        freeListInsert(new_meta);

        temp->size = size;
        temp->isUsed = 1;
        void* ptr = (void*)temp + sizeof(metadata_t);
        freeListRemove(temp);
        return ptr;
      }
      
    }
    temp = temp->next;
  }

  // Increasing heap:
  metadata_t *meta = sbrk(sizeof(metadata_t)); 
  meta->size = size;
  meta->isUsed = 1;

  void* ptr = sbrk(size);

  return ptr;
}


/**
 * Deallocate space in memory
 *
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void *ptr) { // NOTE TO SELF! Not currently freeing last metadata if its at the top of the heap. 
  // implement free
  if (ptr == NULL) return;
  metadata_t* meta = ptr - sizeof(metadata_t);
  meta->isUsed = 0;
  // if (ptr == (sbrk(0) - meta->size)) sbrk(-1 * (meta->size + sizeof(metadata_t)));
  if (meta->size != 0) freeListInsert(meta);
}

/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size) {
  // Edge cases: 
  if (size == 0) {
    free(ptr);
    return NULL;
  }
  if (ptr == NULL) return malloc(size);

  metadata_t* chunk_meta = ptr - sizeof(metadata_t);

  void* to_return;
  if (size <= chunk_meta->size) {
    return ptr;
  } else {
    to_return = malloc(size);
    if (to_return == NULL) return to_return;
    memcpy(to_return, ptr, chunk_meta->size);
    free(ptr);
    return to_return;
  }
  
  return NULL;
}
