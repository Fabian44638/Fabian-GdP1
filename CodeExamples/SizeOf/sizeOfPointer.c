/* Prints size of pointers to the console  */
/* Note: results depend on machine model AND compiler */

#include <stdio.h>
#define OK 0

struct pos {
    int row;
    int col;
};

int main() {
    printf("         char*:\t%02ld Bits\n", (long unsigned int) sizeof(char*)*8); 
    printf("          int*:\t%02ld Bits\n", (long unsigned int) sizeof(int*)*8); 
    printf("         void*:\t%02ld Bits\n", (long unsigned int) sizeof(void*)*8); 
    printf("   struct pos*:\t%02ld Bits\n", (long unsigned int) sizeof(struct pos*)*8); 

    return OK;
}
