#include <stdio.h>
#include <stdbool.h>

#include "readInteger.h"
#include "euklid.h"

extern int ggT(int a, int b);

// Funktionen
int readInteger(char* prompt, int* n) {

    bool erfolg = false;
    int versuch = 1;    // Anzahl der Versuche

    // Dummy call to ggT() in order to test static
    // Compiler error:
    // readInteger.c:19: undefined reference to `ggT'
    // collect2: ld returned 1 exit status
    //ggT(456,34);

    while (!erfolg && versuch <= MAXVERSUCH ) {
        printf("%s",prompt);
        fflush(stdout);

        if(scanf("%d", n) != 1) {
            versuch++;
            printf("Sie muessen hier eine Zahl eingeben!!!\n");
            while(getchar() != '\n');
            continue;
        } else {
            erfolg = true;
            while(getchar() != '\n');
        }
    }

    // Stelle fest, wieso wir Schleife verlassen haben
    if (!erfolg) {
        if(debug) {
            fprintf(stderr,"Fehler bei der Eingabe einer Zahl\n");
        }
        return ERROR;
    } else {
        if(debug) {
            fprintf(stderr,"Erfolg beim Versuch Nr %d\n", versuch);
        }
        return OK;
    }
}
