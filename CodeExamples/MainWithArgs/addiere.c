#include <stdio.h>

// #define OK 0
// #define ERROR 1
//
// Wir verwenden stattdessen die Standard-Definitionen
//  EXIT_SUCCESS 0
//  EXIT_FAILURE 1
// aus stdlib.h

#include <stdlib.h>

int main (int argc, char * argv[]) {
  if (argc != 3) {
    printf("Bitte zwei Zahlen eingeben:\n");
    printf("Beispiel:  %s 2 3 \n", argv[0]);
    return EXIT_FAILURE;
  }
  printf("%s + %s = %d \n",
      argv[1],
      argv[2],
      atoi(argv[1]) + atoi(argv[2]) );

  return EXIT_SUCCESS;
}

