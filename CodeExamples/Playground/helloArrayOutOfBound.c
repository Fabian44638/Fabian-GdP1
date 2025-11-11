#include <stdio.h>

int main() {
    char meldung[] = "Hel";
    char dummy2[] = "o World!";

    //meldung[3] = 'l';  // remove 0 termination
    dummy2[-1] = 'l';    // array out of bounds access

    printf("%s\n", meldung);
    printf("%s\n", dummy2);
    return 0;
}

