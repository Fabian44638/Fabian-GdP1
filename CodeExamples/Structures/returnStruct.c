// How is a structure/record returned by a function

#include<stdio.h>
#include<stdlib.h>

struct pos {
  int x;
  int y;
};

struct pos inc_x(struct pos p){
  struct pos r;
  r.x = p.x+1;
  r.y = p.y;

  return r;
}

int main(void){
  struct pos p1={0x11223344,0x55667788};
  struct pos p2;

  p2 = inc_x(p1);

  printf("p2=(%x,%x)\n", p2.x, p2.y);

  return EXIT_SUCCESS;
}

