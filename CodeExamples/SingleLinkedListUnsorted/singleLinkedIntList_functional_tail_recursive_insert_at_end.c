// We provide iterative and tail-recursive versions for inserting at the end
//
// Demonstrate, that the compiler generates a tight loop for list_insert_end_aux_V1,
// if the optimizer flag -O2 for gcc is specified.
//
// Configure
//    CFLAGS += -O2 -fno-inline-small-functions
// in the Makefile
//
// make clean; make
// gdb -q  -ex "disassemble list_insert_end_aux_V1" -ex "quit" bin/singleLinkedIntList_functional_tail_recursive_demo7
//
// gdb -q  -ex "disassemble list_insert_end_aux_V2" -ex "quit" bin/singleLinkedIntList_functional_tail_recursive_demo7
//
// gdb -q  -ex "disassemble list_insert_end_iter" -ex "quit" bin/singleLinkedIntList_functional_tail_recursive_demo7
//

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "singleLinkedIntList_type.h"

// Variants for inserting at the end
extern node_t* list_insert_end_deep_rec(node_t* node, int data);

extern node_t* list_insert_end_V1(node_t*, int);
extern node_t* list_insert_end_aux_V1(node_t*, int, node_t*);

extern node_t* list_insert_end_V2(node_t*, int);
extern node_t* list_insert_end_aux_V2(node_t*, int, node_t*);

extern node_t* list_insert_end_iter(node_t*, int);

// Auxiliary functions
extern node_t* list_create_node(int data);
extern void list_dump(node_t *node);
extern node_t* list_free(node_t* node);

// --------------------------------------------------------------
// Some variants for inserting data at the end of a linked list
// --------------------------------------------------------------

// Dispatcher for the various versions
node_t* list_insert_end(node_t* node, int data) {
  //return list_insert_end_deep_rec(node,data);
  //return list_insert_end_V1(node,data);
  return list_insert_end_V2(node,data);
  //return list_insert_end_iter(node,data);
}


// Deep recursive version
node_t* list_insert_end_deep_rec(node_t* node, int data) {
  // Is the list empty
  if (node == NULL) {
    return list_create_node(data);
  } else {
    // Recursive call
    node -> next = list_insert_end_deep_rec(node->next, data);
    return node;
  }
}

// ----------------------------------------------------
// Tail recursive version V1
// ----------------------------------------------------
// A wrapper for the tail recursive version with the usual functional interface.
node_t* list_insert_end_V1(node_t* node, int data) {
  return list_insert_end_aux_V1(node, data, node);
}

node_t* list_insert_end_aux_V1(node_t* cur, int data, node_t* res) {
  if (cur == NULL){
    return list_create_node(data);
  }

  if (cur->next != NULL){
    // tail recursive call
    return list_insert_end_aux_V1(cur->next, data, res);
  }

  // cur->next == NULL
  // We are at the last node
  cur -> next = list_create_node(data);
  return res;
}


// ----------------------------------------------------
// Tail recursive version V2
// ----------------------------------------------------
// A variant of the *_aux_V1 function with a single return statement, only.
// * Note: we need to use a proper 'if-else style' here.
// * Check, that the compiler can still perform Tail Call Optimization (TCO).
node_t* list_insert_end_V2(node_t* node, int data) {
  return list_insert_end_aux_V2(node, data, node);
}

node_t* list_insert_end_aux_V2(node_t* cur, int data, node_t* res) {
  if (cur == NULL){
    res = list_create_node(data);
  } else {
    if (cur->next != NULL){
      // Does this still qualify as a tail recursive call?
      res = list_insert_end_aux_V2(cur->next, data, res);
      // Check the disassembled code and behold:
      // res = some_rec_call; followed by return res; --> return some_rec_call
      // The -O2 optimizer recognizes this pattern.
    } else {
      // We are at the last node
      cur -> next = list_create_node(data);
    }
  }
  return res;
}

// ----------------------------------------------------
// Iterative version
// ----------------------------------------------------
node_t* list_insert_end_iter(node_t* cur, int data) {
  node_t *res = cur; // init result
  // Is the list empty
  if (cur == NULL) {
    res = list_create_node(data);
  } else {
    // List is not empty: go to last node
    while (cur->next != NULL) {
      cur = cur -> next;
    }
    // We are at the last node
    cur -> next = list_create_node(data);
  }
  return res;
}


//--------------------------------------------------------------
// Additional functions, which are not in the focus of this demo
//--------------------------------------------------------------

// Create a new node and return its pointer
node_t* list_create_node(int data) {
  node_t* newnode;
  // Create a new node for data
  if ((newnode = malloc(sizeof(node_t))) == NULL) { 
    fprintf(stderr,"list_create_node: Unable to create a new data node\n");
    // Here we bail out without any attempts for recovery
    // This is a very simplistic strategy
    exit(EXIT_FAILURE);
  } else {
    // Initialize node
    newnode -> data = data;
    newnode -> next = NULL;
    return  newnode;
  }
}

// Dump all nodes of a list inclusive information about addresses
// Tail-recursive version
void list_dump(node_t *node) {
  if (node == NULL) {
    return;
  } else {
    printf("Node at %p: data = %d next = %p\n",
        node, node->data, node -> next);
    // Recursive call
    list_dump(node->next);
  }
  return;
}

// Free memory of all nodes in a list
// Recursive version
// Note:
// The interface allows for writing e.g.
// mylist = list_free(mylist);
// which assigns a NULL to the pointer mylist right away.
// This helps in avoiding dangling pointers.
//
// Tail-recursive version
node_t* list_free(node_t* node) {
  if ( node != NULL) {
    node_t* tmp = node->next;
    // Free memory of current node
    free(node);
    // Recursive call
    list_free(tmp);
  }
  return NULL;
}

