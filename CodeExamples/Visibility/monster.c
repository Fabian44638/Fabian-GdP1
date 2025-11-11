#include<stdio.h>

int main() {
  int i=10;

  printf("Outer i=%i\n", i);

  for(int i=0; i<5; i++){
    int i=1;

    i=i*2;
    printf("Innermost i=%i\n", i);
  }

  printf("Outer i=%i\n", i);
  return 0;
}
