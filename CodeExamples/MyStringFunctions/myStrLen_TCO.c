// Tail-call optimization for function strlen
// (C) Franz Regensburger, THI, 2023
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // zum Vergleich

// Deep recursive
size_t my_strlen_rec(const char * s) {
  size_t len = 0;
  if (*s != '\0'){
    len = 1 + my_strlen_rec(s+1);
  }
  return len;
}

// Tail recursive
size_t my_strlen_tail_rec_aux(const char * s, size_t len) {
  if (*s != '\0'){
    len = my_strlen_tail_rec_aux(s+1, len+1);
    //len = my_strlen_tail_rec_aux(s++, len++); // possible variant?
    //len = my_strlen_tail_rec_aux(++s, ++len); // what about this?
  }
  return len;
}

size_t my_strlen_tail_rec(const char * s) {
  return my_strlen_tail_rec_aux(s,0);
}

// Loop with pointer
size_t my_strlen_iter_ptr(const char * s) {
  size_t len = 0;
  while (*s != '\0') {
    s = s+1;
    len = len+1;
  }
  return len;
}

// Loop with array index
size_t my_strlen_iter_idx(const char * s) {
  size_t len = 0;
  size_t i = 0;
  while (s[i] != '\0') {
    i++;
    len++;
  }
  return len;
}


//--
// Dispatcher for the various versions
size_t my_strlen(const char * s) {
  //return my_strlen_rec(s);
  //return my_strlen_tail_rec(s);
  return my_strlen_iter_ptr(s);
  //return my_strlen_iter_idx(s);
}

int main(void) {
  {
    char *s1 = "1234567";
    char *s2 = "";
    printf("my_strlen(\"%s\") = %d\tStandard liefert:%d\n",
        s1, (int) my_strlen(s1), (int) strlen(s1));
    printf("my_strlen(\"%s\") = %d\tStandard liefert:%d\n",
        s2, (int) my_strlen(s2), (int) strlen(s2));
  }

  return EXIT_SUCCESS;
}
