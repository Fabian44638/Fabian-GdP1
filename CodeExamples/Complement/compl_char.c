#include<stdio.h>
#include<stdlib.h>

int main(void){

  char x = -(1<<7);
  char y = -x;

  printf("Anzahl Bits für char x: %ld\n", sizeof(x)*8);
  printf("Anzahl Bits für char y: %ld\n", sizeof(y)*8);

  printf("x = %d, y=%d\n", x,y);

  printf("(x == y) = %s\n", (x==y)?"true":"false");

  return EXIT_SUCCESS;

  // Uups:  x == -y
  // Note:
  // This effect is OK
  // The two's complement is subject to this property.
  //
}
