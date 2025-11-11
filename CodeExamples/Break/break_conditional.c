#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>

int main() {
  printf("Before conditional\n");
  if (true) {
    printf("In conditional at pos 1\n");
    break;
    printf("In conditional at pos 2\n");
  }
  printf("After conditional\n");

  return EXIT_SUCCESS;
}

/*
gcc -g -Wall break_conditional.c -o bin/break_conditional
break_conditional.c: In function ‘main’:
break_conditional.c:9:5: error: break statement not within loop or switch
     break;
     ^~~~~
Makefile:36: recipe for target 'bin/break_conditional' failed
make: *** [bin/break_conditional] Error 1
*/
