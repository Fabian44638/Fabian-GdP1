#include <stdio.h>

int main() {
  int  x = 0x11223344;
  char name[] = "Philomena";
  int  y = 0x55667788;

  printf("Mein Name lautet %s\n", name);
  printf("addr x:    %p\n", &x);
  printf("addr y:    %p\n", &y);
  printf("addr name: %p\n", name);
  printf("addr main: %p\n", main);

  x++; y++;
  name[1] = 0x65;
  printf("Mein Name lautet %s\n", name);
  return 0;
}
