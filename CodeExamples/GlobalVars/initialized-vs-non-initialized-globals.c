#include<stdio.h>
#include<stdlib.h>

int gv_init  = 0x11223344;    // initialized global variable (see object dump)
int gv_non_init;              // non-initialized variable    (set to 0)

int main(void){
  printf("gv_init= %x\n", gv_init);
  printf("gv_non_init = %x\n", gv_non_init);


  return EXIT_SUCCESS;
}
