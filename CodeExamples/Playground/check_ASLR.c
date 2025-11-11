// Demonstrate effect of
// # Disable ASLR in the running kernel
// On Linux:
// echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
//
// On MacOS disable ASLR by removinhg the PIE flag from the Mach binary
// ./change_mach_o_flags.py --no-pie bin/check_ASLR

#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("main starts at %p\n", main);
    return 0;
}

