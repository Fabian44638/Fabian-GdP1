#include <stdio.h>
#include <stdlib.h>

void  zaehlmich(){
  //static int zaehler = 6;
  static int zaehler;
  printf("Das ist der %d-te Aufruf\n",zaehler);
  zaehler++;
}

int main () {
  int i;
  for (i=0; i<5;i++){
    zaehlmich();
  }
  return EXIT_SUCCESS;
}
