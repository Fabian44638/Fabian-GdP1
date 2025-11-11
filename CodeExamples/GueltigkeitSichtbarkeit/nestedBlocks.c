#include <stdio.h>
#include <stdlib.h>

int main () {
  // local variables
  int i,k;
  
  i =0x11223344;
  k =0xaabbccdd;
  printf("Outer block before inner block: &i=%p i = %x &k=%p k = %x\n",
      &i, i, &k, k);
  {
    // inner block
    k =0x11111111;
    i =0x33333333;
    printf("Inner block pre:                &i=%p i = %x &k=%p k = %x\n",
       &i, i, &k, k);
    int i;
    i =0x55667788;
    printf("Inner block post:               &i=%p i = %x &k=%p k = %x\n",
       &i, i, &k, k);
  }
  printf("Outer block after inner block:  &i=%p i = %x &k=%p k = %x\n",
      &i, i, &k, k);

  return EXIT_SUCCESS;
}
