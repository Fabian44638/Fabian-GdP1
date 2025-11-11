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

  { // A dummy block for better separation of concerns

    // START of computation:
    // In case of an error we use a nasty goto in order to break out of the normal
    // sequence of computation.
    printf("In the block at pos 1\n");
    // Simulate a potentially failing computation with
    // error code rnum == 1
    if (rnum == 1){
      printf("Simulated error %d occurred\n", rnum);
      goto error_handling; // leave the block via goto
    }

    printf("In the block at pos 2\n");
    // Simulate a potentially failing computation with
    // error code rnum == 2
    if (rnum == 2){
      printf("Simulated error %d occurred\n", rnum);
      goto error_handling; // leave the block via goto
    }

    printf("In the block at pos 3\n");
    // Simulate a potentially failing computation with
    // error code rnum == 3
    if (rnum == 3){
      printf("Simulated error %d occurred\n", rnum);
      goto error_handling; // leave the block via goto
    }

    printf("At the end of the block; all computations succeeded.\n");
  }

error_handling:
  printf("After block:\n");
  if (rnum > 0){
    // Handle error according to its code
    printf("Computation failed. Bailing out with error code %d\n", rnum);
  } else {
    printf("Returning with success code %d\n", rnum);
  }

  return rnum;
}
