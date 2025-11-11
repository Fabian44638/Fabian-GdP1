#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>

int main() {
  printf("Before loop\n");
  while (true) {
    printf("In loop at pos 1\n");
    break;
    printf("In loop at pos 2\n");
  }
  printf("After loop\n");

  return EXIT_SUCCESS;
}
