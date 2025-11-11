#include <stdio.h>

// If the program's output is 255, the C-compiler treats
// type 'char' as 'unsigned char'.
//
// However, if the program's output is -1, the C-compiler treats
// type 'char' as 'signed char'.

int main(){
  char x = 255;

  printf("x=%d\n", x);
  return 0;
}
