#include<stdio.h>

int g1;
int g2;
int *pi1;
int *pi2;

int main(void){
    int tmp = 5;
    printf("Address of outer tmp: %p\n", &tmp);
    {
        g1 = tmp;
        pi1 = &tmp;

        int tmp; // <--- here starts the visibility of the inner tmp
       
        printf("Address of inner tmp: %p\n", &tmp);
        
        pi2 = &tmp;
        tmp = 2;
        g2 = tmp;
    }

    printf("g1=%d\n", g1);
    printf("pi1=%p\n", pi1);
    printf("pi2=%p\n", pi2);
    printf("g2=%d\n", g2);

    return 0;
}
