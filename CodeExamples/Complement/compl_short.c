#include<stdio.h>
#include<stdlib.h>

int main(void){

  short x = -(1<<15);  // -32768;
  short y = -x;

  printf("Anzahl Bits für short x: %ld\n", sizeof(x)*8);
  printf("Anzahl Bits für short y: %ld\n", sizeof(y)*8);

  printf("x = %d, y=%d\n", x,y);

  printf("(x == y) = %s\n", (x==y)?"true":"false");

  return EXIT_SUCCESS;

  // Uups:  x == -y
  // Note:
  // This effect is OK
  // The two's complement is subject to this property.
  //
}
