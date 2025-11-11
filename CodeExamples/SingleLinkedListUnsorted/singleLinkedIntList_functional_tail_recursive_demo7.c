// Demo of various versions for inserting an element at the end of the list

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "singleLinkedIntList_type.h"

extern node_t* list_insert_end(node_t* anchor, int data);
extern void list_dump(node_t *anchor);
extern node_t* list_free(node_t* anchor);

// ----------------------------------------------------------------
// MAIN
// ----------------------------------------------------------------

int main () {
  node_t* mylist=NULL;

  printf("\nDemo 7: inserting at the end\n");

  list_dump(mylist); printf("\n");

  mylist = list_insert_end(mylist, 10);
  list_dump(mylist); printf("\n");

  mylist = list_insert_end(mylist, 11);
  list_dump(mylist); printf("\n");

  mylist = list_insert_end(mylist, 12);
  list_dump(mylist); printf("\n");

  mylist = list_insert_end(mylist, 13);
  list_dump(mylist); printf("\n");

  // Free the list; assignment sets anchor to NULL
  mylist = list_free(mylist);

  // ---------------------------------------
  return EXIT_SUCCESS;
}
