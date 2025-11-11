#include<stdio.h>
#include<stdlib.h>

int main(void){

  int x = -(1<<31);
  int y = -x;

  printf("Anzahl Bits für int x: %ld\n", sizeof(x)*8);
  printf("Anzahl Bits für int y: %ld\n", sizeof(y)*8);

  printf("x = %d, y=%d\n", x,y);

  printf("(x == y) = %s\n", (x==y)?"true":"false");

  return EXIT_SUCCESS;

  // Uups:  x == -y
  // Note:
  // This effect is OK
  // The two's complement is subject to this property.
  //
  // Very interesting: for type int, we get a warning during compilation.
  //
  //$ make
  // gcc -g -Wall compl_int.c -o bin/compl_int
  // compl_int.c: In function ‘main’:
  // compl_int.c:6:11: warning: integer overflow in expression [-Woverflow]
  // int x = -(1<<31);
  //         ^
  //
  //
}
