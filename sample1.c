#include <stdlib.h>
// #include "alloc.c"

int main() {

const int TEST_SIZE = 10240;
  char *buffer = malloc(TEST_SIZE);

  int i;
  for (i = 0; i < TEST_SIZE; i++) {
    *buffer = 'a';
  }

  // free(buffer);
  return 0;

//   void *a = malloc(256);  // 256 == 0x 100 bytes
//   void *b = malloc(256);
//   void *c = malloc(128);
//   void *d = malloc(256);
//   void *e = malloc(128);
//   /* Line 6 */
  
//   free(a);
//   free(b);
//   free(d);

//   /* Line 10 */
//   void *r1 = malloc(10);
//   void *r2 = malloc(10);
//   void *r3 = malloc(300);
//   void *r4 = malloc(250);  
}