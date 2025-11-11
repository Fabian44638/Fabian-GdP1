/*
 * from https://stackoverflow.com/questions/14737104/
 * what-is-the-default-c-mode-for-the-current-gcc-especially-on-ubuntu
 */

#include <stdio.h>

int main(void) {
#ifdef __STDC_VERSION__
  printf("__STDC_VERSION__ = %ld \n", __STDC_VERSION__);
#endif
#ifdef __STRICT_ANSI__
  puts("__STRICT_ANSI__");
#endif
  return 0;
}
