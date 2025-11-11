/*
See
http://teaching.idallen.com/dat2343/10f/notes/040_overflow.txt

Calculating Overflow Flag: Method 1
-----------------------------------

Overflow can only happen when adding two numbers of the same sign and
getting a different sign.  So, to detect overflow we don't care about
any bits except the sign bits.  Ignore the other bits.

With two operands and one result, we have three sign bits (each 1 or
0) to consider, so we have exactly 2**3=8 possible combinations of the
three bits.  Only two of those 8 possible cases are considered overflow.
Below are just the sign bits of the two addition operands and result:

       ADDITION SIGN BITS
    num1sign num2sign ADDsign
   ---------------------------
        0       0       0
 *OVER* 0       0       1 (adding two positives should be positive)
        0       1       0
        0       1       1
        1       0       0
        1       0       1
 *OVER* 1       1       0 (adding two negatives should be negative)
        1       1       1

We can repeat the same table for subtraction.  Note that subtracting
a positive number is the same as adding a negative, so the conditions that
trigger the overflow flag are:

      SUBTRACTION SIGN BITS
    num1sign num2sign SUBsign
   ---------------------------
        0       0       0
        0       0       1
        0       1       0
 *OVER* 0       1       1 (subtract negative is same as adding a positive)
 *OVER* 1       0       0 (subtract positive is same as adding a negative)
        1       0       1
        1       1       0
        1       1       1
*/

#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<limits.h>

int main() {
  assert(sizeof(long long int) == 8);
  // The value 0x 80 00 00 00 00 00 00 00 is the
  // minimal integer for signed integer with 8 byte
  assert(LLONG_MIN == 0x8000000000000000);
  long long int x   = LLONG_MIN;
  long long int y   = LLONG_MIN;

  printf("x=%lld y=%lld\n", x, y);

  long long int res;

  // Subtraction: we expect result 0; overflow bit is not set.
  // Use debugger gdb and check with 'print $eflags' after the assignment.
  res = x - y;

  // Addition: we expect an error; overflow bit set.
  // Use debugger gdb and check with 'print $eflags' after the assignment
  res = x + y;

  printf("x+y: res=%lld\n", res);

  // Note:
  // See: https://stackoverflow.com/questions/69104632/
  //              compiler-flags-for-checking-integer-overflow
  //
  // Compiling with the gcc flag -fsanitize=undefined will abort
  // the addition statement 'x+y' above during runtime
  //
  // gcc -g -Wall overflow_64_8byte.c -fsanitize=undefined -o bin/overflow_64_8byte
  //
  // Check the assembler code generated with/without option -fsanitize=undefined
  //
  // gcc -S -Wall overflow_64_8byte.c
  // view overflow_64_8byte.s
  //
  // gcc -S -Wall overflow_64_8byte.c -fsanitize=undefined
  // view overflow_64_8byte.s

  printf("x-y: res=%lld\n", res);

  return 0;
}
