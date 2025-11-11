// We provide deep recursive, tail-recursive and iterative versions
// for deleting at the end
//
// Demonstrate, that the compiler generates a tight loop for list_remove_tail_aux,
// if the optimizer flag -O2 for gcc is specified.
//
// make clean; make
// gdb -q  -ex "disassemble list_remove_tail_tail_rec_aux" -ex "quit" bin/singleLinkedIntList_functional_tail_recursive_demo8
//
// gdb -q  -ex "disassemble list_remove_tail_iter" -ex "quit" bin/singleLinkedIntList_functional_tail_recursive_demo8
//
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "singleLinkedIntList_type.h"

// Variants for deleting at the end
extern node_t* list_remove_tail_deep_rec(node_t* node);

extern node_t* list_remove_tail_tail_rec(node_t* node);
extern node_t* list_remove_tail_tail_rec_aux(node_t* node, node_t* first, node_t* last);

extern node_t* list_remove_tail_iter(node_t* node);

// Variants for inserting at the end
extern node_t* list_insert_end_V2(node_t*, int);
extern node_t* list_insert_end_aux_V2(node_t*, int, node_t*);

// Auxiliary functions
extern node_t* list_create_node(int data);
extern void list_dump(node_t *node);
extern node_t* list_free(node_t* node);

// --------------------------------------------------------------
// Some variants for deleting the element at the end of the list
// --------------------------------------------------------------

// Dispatcher for the various versions
node_t* list_remove_tail(node_t* node) {
  //return list_remove_tail_deep_rec(node);
  return list_remove_tail_tail_rec(node);
  //return list_remove_tail_iter(node);
}

// Deep recursive version
node_t* list_remove_tail_deep_rec(node_t* node){
  node_t* res = node;
  if (node != NULL) {
    if (node->next != NULL) {
      // Recursive call
      node->next = list_remove_tail_deep_rec(node->next);
    } else {
      // Remove last node
      free(node);
      res = NULL;
    }
  }
  return res;
}

// ----------------------------------------------------
// Tail recursive version
// ----------------------------------------------------
// A wrapper for the tail recursive version with the usual functional interface.
// Check, that the compiler can still perform Tail Call Optimization (TCO).
node_t* list_remove_tail_tail_rec(node_t* node){
  return list_remove_tail_tail_rec_aux(node, node, NULL);
}

node_t* list_remove_tail_tail_rec_aux(node_t* cur, node_t* res, node_t* last){
  if (cur != NULL){
    if (cur->next != NULL){
      // Does this still qualify as a tail recursive call?
      res = list_remove_tail_tail_rec_aux(cur->next, res, cur);
      // Check the disassembled code and behold:
      // res = some_rec_call; followed by return res; --> return some_rec_call
      // The -O2 optimizer recognizes this pattern.
    } else {
      // Remove last node
      free(cur);

      if (last == NULL){
        // List had only a single element
        res = NULL;
      } else {
        // List had more than one element
        last -> next = NULL;
      }
    }
  }
  return res;
}

// ----------------------------------------------------
// Iterative version
// ----------------------------------------------------
node_t* list_remove_tail_iter(node_t* cur){
  node_t *res = cur; // init result
  node_t* last = NULL;
  if (cur != NULL){
    // List is not empty: go to last node
    while (cur->next != NULL) {
      last = cur;
      cur = cur -> next;
    }
    // Remove last node
    free(cur);

    if (last == NULL){
      // List had only a single element
      res = NULL;
    } else {
      // List had more than one element
      last -> next = NULL;
    }
  }
  return res;
}


// --------------------------------------------------------------
// Some variants for inserting data at the end of a linked list
// --------------------------------------------------------------

// Dispatcher for the various versions
node_t* list_insert_end(node_t* node, int data) {
  return list_insert_end_V2(node,data);
}

// ----------------------------------------------------
// Tail-recursive version
// ----------------------------------------------------
// A wrapper for the tail recursive version with the usual functional interface
node_t* list_insert_end_V2(node_t* node, int data) {
  return list_insert_end_aux_V2(node, data, node);
}

node_t* list_insert_end_aux_V2(node_t* cur, int data, node_t* res) {
  if (cur == NULL){
    res = list_create_node(data);
  } else {
    // By condition above and (inv): cur and first are both != NULL
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

