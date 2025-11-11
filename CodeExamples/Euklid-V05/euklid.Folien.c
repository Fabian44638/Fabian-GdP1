#define MAXVERSUCH 3
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

extern int ggT(int, int);
extern int readInteger(char*, int*);

int main (void) {
  int i1, i2, theGGT;

  readInteger("i1= ", &i1);
  readInteger("i2= ", &i2);
  theGGT = ggT(i1, i2);
  printf ("ggT= %d\n", theGGT);
  return 0;
}

/* ------------------------------------------------
 * Hilfsfunktionen
 * ------------------------------------------------
 */

int ggT(int a, int b) {
  while (a != b) {
    if (a < b)
      b = b - a;
    else
      a = a - b;
  }
  return a;
}

int readInteger(char* prompt, int* n) {
  bool erfolg = false;
  int versuch = 1;    // Anzahl der Versuche

  while (!erfolg && versuch <= MAXVERSUCH ) {
    printf("%s",prompt);
    fflush(stdout);

    if(scanf("%d", n) != 1) {
      versuch++;
      printf("Sie müssen hier eine ganze Zahl eingeben!!!\n");
      while(getchar() != '\n');
      continue;
    } else {
      erfolg = true;
      while(getchar() != '\n');
    }
  }

  if (!erfolg) {
    return EXIT_FAILURE;
  } else {
    return EXIT_SUCCESS;
  }
}

