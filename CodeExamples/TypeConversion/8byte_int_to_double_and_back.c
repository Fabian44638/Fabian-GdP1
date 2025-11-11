// Convert 8 byte signed integer to double and back

#include <stdio.h>

int main() {

    long long int i1, i3;
    double d2;

    printf("Size of i1 in bit: %zd\n", 8 * sizeof(i1));
    printf("Size of d2 in bit: %zd\n", 8 * sizeof(d2));
    printf("Size of i3 in bit: %zd\n", 8 * sizeof(i3));

    //i1 = 0x7FFFFFFFFFFFFFFF;  // largest positive signed 8 byte integer
    //                          // 2^63 = 9223372036854775808
    i1 = 0x7FFFFFFFFFFFFF00;    // some large positive signed 8 byte integer
                                // 2^63 -256 = 9223372036854775552

    d2 = i1;                    // convert to double due to assignment

    i3 = d2;                    // convert back to int due to assignment

    // Print the result: behold values of i1 and i2!
    // Start gdb and set a breakpoint in the next line of code
    //    dump memory via: x/24xb &i1
    //             or via: x/3xg  &i1    
    printf("\nDisplaying values corresponding to type of variable\n");
    printf("i1 =  %lld\n", i1);
    printf("d2 =  %f\n"  , d2);
    printf("i3 = %lld\n" , i3);

    return 0;
}
