// Example for a dynamically allocated 2-dimensional array of integers
// We use the storage scheme of an array of pointers that point
// to arrays of int (APA)

#include<stdio.h>
#include<stdlib.h>

#define ROWS 3
#define COLS 4
#define OK 0
#define ERROR 1

void free_array(int** pa, int rows){
  // For each row free the array of its columns
  for (int row = 0; row < rows; row++) {
    if (pa[row] == NULL) { break; }
    free(pa[row]);
  }
  // Free the array of pointers
  free(pa);
}

int main (void) {
  // Loop vars
  int row,col;

  // A pointer to an array of pointers that point to arrays of int
  int** da;

  // Allocate memory for array of pointers with ROWS elements
  // Store address in base pointer da
  // Use calloc in order to initialize the array with NULL pointers!
  // This is crucial for the partial cleanup.
  da = calloc(ROWS , sizeof(int*));
  if (da == NULL) {
    printf("Out of memory\n");
    // nothing to cleanup
    return ERROR;
  }

  // Allocate array of colums for each row and store pointers in array
  for (row = 0; row < ROWS; row++) {
    da[row] = malloc(COLS * sizeof(int));
    if (da[row] == NULL) {
      printf("Out of memory\n");
      // do partial cleanup
      free_array(da, ROWS); 
      return ERROR;
    }
  }

  // For each row fill the 1-dimensional array of its columns
  for ( row = 0; row < ROWS; row++) {
    for (col = 0; col < COLS; col++) {
      da[row][col] = 10*(row+1) + (col+1);
      // Note the syntax
      // It is parsed like this: (da[row])[col]
      // Alternativ access to the same array element
      //*(*(da+row)+col) = 10*(row+1) + (col+1);
    }
  }

  // Print the array
  printf("Base pointer da at %p points to %p\n",&da, da); 
  for ( row = 0; row < ROWS; row++) {
    printf("da[%d] at %p\n",row,&da[row]); 
    for (col = 0; col < COLS; col++) {
      printf("da[%d][%d] = %d at %p\n",
          row,col,
          da[row][col],
          &(da[row][col])
          // Alternative syntax
          // *(da+row) + col
          ); 
    }
    printf("\n");
  }

  // In the end free the allocated memory
  free_array(da, ROWS); 

  return OK;
}
