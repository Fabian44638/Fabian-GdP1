#include<stdio.h>

int main(void){
    int num, intWert;
    float floatWert;

    // Achtung: scanf prüft das Format "%d,%f" relativ genau:
    // Die Anzahl der Lehrzeichen nach dem ',' ist egal.
    // Vor dem ',' darf aber kein Lehrzeichen eingegeben werden!
    // 
    //   Akzeptiert werden folgende Eingabe:
    //
    //   '10,3.14'
    //   '10, 3.14'
    //   '10,  3.14'
    //   '10,   3.14'
    //   '10,    3.14'
    //   ...
    //
    //   Nicht akzeptiert werden aber folgende Eingaben
    //   '10 ,3.14'
    //   '10  ,3.14'
    //   '10 , 3.14'
    
    printf("Bitte zwei Werte eingeben: Template = \"%%d,%%f\"\n");
    num = scanf("%d,%f", &intWert, &floatWert);

    if(num!=2) {
        printf("Fehler bei der Eingabe\n");
    } else {
        printf("Eingabe: intWert= %d, floratWert= %f\n", intWert, floatWert);
    }

    return 0;
}
