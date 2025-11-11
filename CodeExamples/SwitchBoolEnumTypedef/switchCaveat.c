#include <stdio.h>
#include <stdlib.h>

// Achtung:
// Sobald der erste Case einen Match erzeugt, werden allen Anweisungen
// danach ausgeführt, solange bis das erste break kommt.
// 
// Im Beispiel unten:
// Es werden die Anweisungen aus case 3:
// und danach auch die aus case 4: ausgeführt, obwohl val == 3.
// Es werden also auch Zweige ausgeführt, deren Case-Wert nicht passt!

int main(void){
  int val = 3;

  switch(val) {
    case 1:
      printf("Fall 1\n");
      break;
    case 2:
    case 3:
      printf("Fall 2 oder 3\n");
      //break;
    case 4:
      printf("Fall 4\n");
      break;
    default:
      printf("Zweig default: kein expliziter Fall für %d festgelegt\n", val);
  }
  return EXIT_SUCCESS;
}
