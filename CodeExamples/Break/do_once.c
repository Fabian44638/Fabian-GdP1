#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<time.h>

int main() {
  // Initialize random generator
  srand ( time(NULL) );

  // Create a random number rnum with 0 <= rnum < 4
  // Purpose:
  // 0 -> no error shall occurr
  // 1 -> error with error_code 1 shall occurr
  // 2 -> error with error_code 2 shall occurr
  // 3 -> error with error_code 4 shall occurr

  int rnum = rand() % 4;

  printf("Before block\n");

  // A 'do once' loop models our block of computation.
  // Use these for long sequences of actions that may fail at any time.
  //
  // The break statement replaces a nasty goto statement to some code
  // after the loop, where we do error handling.
  do {
    printf("In the block at pos 1\n");
    // Simulate a potentially failing computation with
    // error code rnum == 1
    if (rnum == 1){
      printf("Simulated error %d occurred\n", rnum);
      break; // leave the block (of the dummy loop)
    }

    printf("In the block at pos 2\n");
    // Simulate a potentially failing computation with
    // error code rnum == 2
    if (rnum == 2){
      printf("Simulated error %d occurred\n", rnum);
      break; // leave the block (of the dummy loop)
    }

    printf("In the block at pos 3\n");
    // Simulate a potentially failing computation with
    // error code rnum == 3
    if (rnum == 3){
      printf("Simulated error %d occurred\n", rnum);
      break; // leave the block (of the dummy loop)
    }

    printf("At the end of the block; all computations succeeded.\n");
  } while (false);

  printf("After block:\n");
  if (rnum > 0){
    // Handle error according to its code
    printf("Computation failed. Bailing out with error code %d\n", rnum);
  } else {
    printf("Returning with success code %d\n", rnum);
  }

  return rnum;
}
