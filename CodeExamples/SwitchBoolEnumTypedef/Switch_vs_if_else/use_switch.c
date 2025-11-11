// Frage: ist switch() schneller als if ... else ?

#include <stdio.h>
#include <stdlib.h>

#define MO 1
#define DI 2
#define MI 3
#define DO 4
#define FR 5
#define SA 6
#define SO 7

int do_selection(int d){
  int res;
  switch(d) {
    case MO ... FR:
      res=0xf1;
      break;
    case SA:
      res=0xf2;
      break;
    case SO:
      res=0xf3;
      break;
    default:
      res=0xf4;
  }
  return res;
}

int main (int argc, char** argv) {
  int wd;

  if(argc !=2){
    printf("usage: %s <1-7>\n", argv[0]);
    exit(1);
  }

  wd = atoi(argv[1]);
  return do_selection(wd);
}

