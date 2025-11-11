/* Prints size of primitive datatypes tp the console  */
/* Note: results depend on machine model AND compiler */

#include <stdio.h>
#define OK 0

int main() {
    printf("         char:\t%3ld Bits\n", (long unsigned int) sizeof(char)*8); 
    printf("        short:\t%3ld Bits\n", (long unsigned int) sizeof(short int)*8); 
    printf("          int:\t%3ld Bits\n", (long unsigned int) sizeof(int)*8); 
    printf("     long int:\t%3ld Bits\n", (long unsigned int) sizeof(long int)*8); 
    printf("long long int:\t%3ld Bits\n", (long unsigned int) sizeof(long long int)*8); 
    printf("        float:\t%3ld Bits\n", (long unsigned int) sizeof(float)*8); 
    printf("       double:\t%3ld Bits\n", (long unsigned int) sizeof(double)*8); 
    printf("  long double:\t%3ld Bits\n", (long unsigned int) sizeof(long double)*8); 

    return OK;
}
