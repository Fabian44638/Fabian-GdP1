#include<stdio.h>
#define FEHLER 42
#define OK 0

int teile_mit_rest(int a, int b, int* p_q, int* p_r){
  if (a<0 || b <= 0){
    return FEHLER;
  }

  int cnt = 0;
  while(a >= b){
    a = a - b;
    cnt++;
  }

  *p_q = cnt;
  *p_r = a;

  return OK;
}

int main(void){
  int x, y, quotient, rest, res_code;
  x = 24;
  y = 9;

  res_code = teile_mit_rest(x, y, &quotient, &rest);
  if (res_code != OK){
    printf("Fehler bei Ausführung von teile_mit_rest\n");
    return FEHLER;
  }

  printf("Ganzzahlig teilen von %d durch %d geht %d mal Rest %d\n", x, y, quotient, rest);
  return OK;
}
