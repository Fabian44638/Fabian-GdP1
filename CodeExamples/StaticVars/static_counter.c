#include<stdio.h>

int g=0x11223344;
int *pi;

void f1(){
  static int s15 = 0x55667788;

  pi = &s15;
  printf("in f1: s15=%x\n", *pi);
  s15++;
}

int main(void){
  for(int j=0; j<5; j++){
    f1();
  }

  //Accessing s15 from outside of f1() via name yields an error
  //printf("s15=%x\n", s15);

  //Accessing s15 via pointer is successfull
  printf("in main: s15=%x\n", *pi);

  return 0;
}

