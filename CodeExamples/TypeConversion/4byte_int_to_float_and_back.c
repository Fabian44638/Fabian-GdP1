// Convert 4 byte signed integer to float and back

#include <stdio.h>

int main() {

    int i1, i3;
    float f2;

    printf("Size of i1 in bit: %zd\n", 8 * sizeof(i1));
    printf("Size of f2 in bit: %zd\n", 8 * sizeof(f2));
    printf("Size of i3 in bit: %zd\n", 8 * sizeof(i3));

    //i1 = 0x7FFFFFFF;  // largest positive signed 4 byte integer
    i1 = 0x7FFFFFF0;    // some large positive signed 4 byte integer

    f2 = i1;            // convert to float due to assignment

    i3 = f2;            // convert back to int due to assignment

    // Print the result: behold values of i1 and i2!
    // Start gdb and set a breakpoint in the next line of code
    //    dump memory via: x/12xb &i1
    //             or via: x/3xw  &i1    
    printf("\nDisplaying values corresponding to type of variable\n");
    printf("i1 =  %d\n", i1);
    printf("f2 =  %f\n", f2);
    printf("i3 = %d\n" , i3);

    return 0;
}
