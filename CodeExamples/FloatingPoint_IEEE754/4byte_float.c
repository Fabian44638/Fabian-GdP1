// Investigate the IEEE 754 storage format of float precision floating point numbers
// See
// https://en.wikipedia.org/wiki/Single-precision_floating-point_format

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <math.h>       /* isnormal */

// A data type for modelling the bitwise representation of IEEE 754
// float values with 4 byte

struct singlerep {
  char nibble1_rep[8+1];
  char nibble2_rep[8+1];
  char nibble3_rep[8+1];
  char nibble4_rep[8+1];
  char nibble5_rep[8+1];
  char nibble6_rep[8+1];
  char nibble7_rep[8+1];
  char nibble8_rep[8+1];

  char all_rep[32+1];
  char sign_rep[1+1];
  char exponent_rep[8+1];
  char fraction_rep[23+1];

  unsigned char nibble1_val;
  unsigned char nibble2_val;
  unsigned char nibble3_val;
  unsigned char nibble4_val;
  unsigned char nibble5_val;
  unsigned char nibble6_val;
  unsigned char nibble7_val;
  unsigned char nibble8_val;

  unsigned char sign_val;
  unsigned short exponent_val;
  unsigned int fraction_val;
};

// ---------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------

void make_bit_string(unsigned int ui, char* buf, int pos){
  buf[pos] = '\0'; // termination
  pos--;
  do {
    if (ui % 2 == 1){
      buf[pos] = '1';
    } else {
      buf[pos] = '0';
    }
    ui = ui >> 1;
    pos--;
  } while (pos >= 0);
}

struct singlerep make_bitrep32(unsigned int ui){
  struct singlerep res;

  // create values by bit masking
  res.nibble1_val  = (ui & 0xf0000000)>>28;
  res.nibble2_val  = (ui & 0x0f000000)>>24;
  res.nibble3_val  = (ui & 0x00f00000)>>20;
  res.nibble4_val  = (ui & 0x000f0000)>>16;
  res.nibble5_val  = (ui & 0x0000f000)>>12;
  res.nibble6_val  = (ui & 0x00000f00)>>8;
  res.nibble7_val  = (ui & 0x000000f0)>>4;
  res.nibble8_val  = (ui & 0x0000000f);

  res.sign_val     = (ui & 0x80000000)>>31;
  res.exponent_val = (ui & 0x7F800000)>>23;
  res.fraction_val = (ui & 0x007FFFFF);

  make_bit_string(res.nibble1_val, res.nibble1_rep, 4);
  make_bit_string(res.nibble2_val, res.nibble2_rep, 4);
  make_bit_string(res.nibble3_val, res.nibble3_rep, 4);
  make_bit_string(res.nibble4_val, res.nibble4_rep, 4);
  make_bit_string(res.nibble5_val, res.nibble5_rep, 4);
  make_bit_string(res.nibble6_val, res.nibble6_rep, 4);
  make_bit_string(res.nibble7_val, res.nibble7_rep, 4);
  make_bit_string(res.nibble8_val, res.nibble8_rep, 4);

  make_bit_string(ui, res.all_rep, 32);
  make_bit_string(res.sign_val, res.sign_rep, 1);
  make_bit_string(res.exponent_val, res.exponent_rep, 8);
  make_bit_string(res.fraction_val, res.fraction_rep, 23);

  return res;
}

float shift_float32(float val, short int ex){
  if (ex == 0){
    // NOP
  } else if (ex > 0) {
    // left shift -> multiply by 2.0
    while (ex > 0) {
      val *= 2.0;
      ex--;
    }
  } else {
    // ex < 0
    // right shift -> devide by 2.0
    while (ex < 0) {
      val /= 2.0;
      ex++;
    }
  }
  return val;
}


bool make_float_from_hex32(char* str, float *fp){
  // We expect something like this as str: c11|51959

  char* p = str;
  int nibble_cnt=0;
  char buf[]="x";

  // Search the end of str and do some checks
  while (*p != '\0'){
    buf[0] = *p;
    if (strstr("01234567890aAbBcCdDeEfF|", buf) == NULL){
      fprintf(stderr,"Hexadecimal value contains illegal character %c\n", (32 <= *p && *p <= 126)?*p:' ');
      return false;
    }

    if (*p != '|'){
      nibble_cnt += 1;
    }
    p++;
  }

  if (nibble_cnt != 8){
    fprintf(stderr,"Hexadecimal value must contain exactly 8 nibbles\n");
    return false;
  }

  // We are at the '\0' of str.
  if (p != str) p--;

  // Compile the hex value from the nibble string ignoring '|' on the way back
  unsigned int ui = 0;
  unsigned shift = 0;
  while(str <= p){
    if (*p != '|'){
      switch(*p){
        case '0':
          break;
        case '1':
          ui += 1* (1U<<shift);
          break;
        case '2':
          ui += 2* (1U<<shift);
          break;
        case '3':
          ui += 3* (1U<<shift);
          break;
        case '4':
          ui += 4* (1U<<shift);
          break;
        case '5':
          ui += 5* (1U<<shift);
          break;
        case '6':
          ui += 6* (1U<<shift);
          break;
        case '7':
          ui += 7* (1U<<shift);
          break;
        case '8':
          ui += 8* (1U<<shift);
          break;
        case '9':
          ui += 9* (1U<<shift);
          break;
        case 'a':
        case 'A':
          ui += 10* (1U<<shift);
          break;
        case 'b':
        case 'B':
          ui += 11* (1U<<shift);
          break;
        case 'c':
        case 'C':
          ui += 12* (1U<<shift);
          break;
        case 'd':
        case 'D':
          ui += 13* (1U<<shift);
          break;
        case 'e':
        case 'E':
          ui += 14* (1U<<shift);
          break;
        case 'f':
        case 'F':
          ui += 15* (1U<<shift);
          break;
      }
      shift += 4;
    }
    p--;
  }

  // Assign the bitpattern of the compiled hexvalue to fp
  unsigned int* uip;
  uip = (unsigned int*) fp;
  *uip = ui; 
  return true;
}

bool make_float_from_bin32(char* str, float *fp){
  // We expect something like this as str
  // 1|10000010|00101010001100101011001

  char* p = str;
  int bit_cnt=0;
  char buf[]="x";

  // Search the end of str and do some checks
  while (*p != '\0'){
    buf[0] = *p;
    if (strstr("01|", buf) == NULL){
      fprintf(stderr,"Binary value contains illegal character %c\n", (32 <= *p && *p <= 126)?*p:' ');
      return false;
    }

    if (*p != '|'){
      bit_cnt += 1;
    }
    p++;
  }

  if (bit_cnt != 32){
    fprintf(stderr,"Binary value must contain exactly 32 bits\n");
    return false;
  }

  // We are at the '\0' of str.
  if (p != str) p--;

  // Compile the binary value from the binary string ignoring '|' on the way back
  unsigned int ui = 0;
  unsigned shift = 0;
  while(str <= p){
    if (*p != '|'){
      switch(*p){
        case '0':
          break;
        case '1':
          ui += 1* (1U<<shift);
          break;
      }
      shift += 1;
    }
    p--;
  }

  // Assign the bitpattern of the compiled hexvalue to fp
  unsigned int* uip;
  uip = (unsigned int*) fp;
  *uip = ui; 
  return true;
}

void usage(char* prog){
  printf("\nUsage:\n");
  printf("%s mode values\n", prog);
  printf("\nwhere mode is one of: -b, -f, -x \n");
  printf("\nExamples for mode -f: 32bit float\n");
  printf("%s -f -345686.40625\n", prog);
  printf("%s -f -3.4568640625E+05\n", prog);
  printf("\nExamples for mode -b: 32bit binary (bit separators '|' are optional) \n");
  printf("%s -b '1|10010001|01010001100101011001101'\n", prog);
  printf("\nExamples for mode -x: 32bit hexadecimal (nibble separators '|' are optional) \n");
  printf("%s -x 'c8a|8cacd'      some normal case\n", prog);
  printf("%s -x '008|00000'      some normal case\n", prog);
  printf("%s -x '7F7|FFFFF'      some normal case\n", prog);
  printf("%s -x '000|00000'      non-normal case +zero\n", prog);
  printf("%s -x '800|00000'      non-normal case -zero\n", prog);
  printf("%s -x '000|00001'      non-normal case +subnormal\n", prog);
  printf("%s -x '007|FFFFF'      non-normal case +subnormal\n", prog);
  printf("%s -x '800|00001'      non-normal case -subnormal\n", prog);
  printf("%s -x '807|FFFFF'      non-normal case -subnormal\n", prog);
  printf("%s -x '7F8|00000'      non-normal case +INF\n", prog);
  printf("%s -x 'FF8|00000'      non-normal case -INF\n", prog);
  printf("%s -x '7FF|FFFFF'      non-normal case +NAN\n", prog);
  printf("%s -x 'FFF|FFFFF'      non-normal case -NAN\n", prog);
  printf("\n");
}

// ---------------------------------------------------------------------
// MAIN
// ---------------------------------------------------------------------

int main(int argc, char **argv) {
  // Check our assumptions about the storage format
  assert(sizeof(float) == 4);
  assert(sizeof(unsigned int) == 4);

  // Variable for storing the 32bit float
  float float_val;

  // Process command line (simple)
  if (argc == 1 || argc > 3){
    usage(argv[0]);
    return EXIT_FAILURE;
  }

  if (strcmp(argv[1],"-f") == 0){
    if(1 != sscanf(argv[2], "%f", &float_val)){
      fprintf(stderr,"Please provide a properly formatted floating point value\n");
      usage(argv[0]);
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1],"-x") == 0){
    if(! make_float_from_hex32(argv[2], &float_val)){
      fprintf(stderr,"Please provide a properly formatted hexadecimal string\n");
      usage(argv[0]);
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1],"-b") == 0){
    if(! make_float_from_bin32(argv[2], &float_val)){
      fprintf(stderr,"Please provide a properly formatted binary string\n");
      usage(argv[0]);
      return EXIT_FAILURE;
    }
  } else {
    printf("Unknown mode\n");
    usage(argv[0]);
    return  EXIT_FAILURE;
  }

  // Some pointer voodoo in order to prevent type conversion of values
  // Note:
  // The plain assignment
  //  unsigned int ui = float_val;
  // enforces a type conversion, where bits are not preserved
  unsigned int* uip;
  uip = (unsigned int*) &float_val;

  // Now, the integer pointer uip points to the variable containing the 
  // 4 byte single variable float_val

  printf("\n");
  if (isnormal(float_val)){
    printf("Single precision float variable formatted with %%.10E (normal):\n\n\t%.10E\n", float_val);
  } else {
    printf("Single precision float variable formatted with %%.10E (non-normal):\n\t%.10E\n", float_val);
  }
  printf("\n");
  printf("Hexadecimal representation in memory (msb first): %04x\n", *uip);

  // Create a string representation of the binary value
  struct singlerep float_rep = make_bitrep32(*uip);

  printf("\n");
  printf("Nibble representation:\n");
  printf("     all: %32s\n" , float_rep.all_rep);
  printf("nibble 1: %4s\n"  , float_rep.nibble1_rep);
  printf("nibble 2: %8s\n"  , float_rep.nibble2_rep);
  printf("nibble 3: %12s\n" , float_rep.nibble3_rep);
  printf("nibble 4: %16s\n" , float_rep.nibble4_rep);
  printf("nibble 5: %20s\n" , float_rep.nibble5_rep);
  printf("nibble 6: %24s\n" , float_rep.nibble6_rep);
  printf("nibble 7: %28s\n" , float_rep.nibble6_rep);
  printf("nibble 8: %32s\n" , float_rep.nibble8_rep);

  // Create a string representation of the IEEE 754 single value
  printf("\n");
  printf("IEEE 754 single components:\n");
  printf("     all: %32s\n", float_rep.all_rep);
  printf("    sign: %1s\n" , float_rep.sign_rep);
  printf("exponent: %9s\n", float_rep.exponent_rep);
  printf("fraction: %32s\n", float_rep.fraction_rep);

  // Recompute the value from the components
  printf("\n");
  printf("Re-computing the IEEE 754 single from its components:\n");

  char sign_factor = (float_rep.sign_val?-1:1);
  printf("           sign_bit: %d\n", float_rep.sign_val);
  printf("        sign_factor: %d\n", sign_factor);

  // Interpretation of the exponent bit pattern
  if (float_rep.exponent_val == 0){
    if (float_rep.fraction_val == 0){
      // Signed zero
      printf("Recomputed value (signed zero): %c0.0\n", sign_factor<0?'-':'+');
    } else {
      // Subnormal number
      short int ex = (-126);
      printf("  exponent (   biased): %d\n", float_rep.exponent_val);
      printf("  exponent (subnormal): %d\n", ex);

      // Compute the exponent by successive division by 2
      float exp_factor = shift_float32(1.0, ex);
      printf("    exponent factor: %.10E\n", exp_factor);

      // Compute the significand by successive division by 2
      float significand = 0.0 + shift_float32(1.0 * float_rep.fraction_val, -23);
      printf(" unshifted fraction: %.10E\n", 1.0*float_rep.fraction_val);
      printf(" 0.shifted fraction: %.10E\n", significand);


      float recomputed_val = 1.0 * sign_factor * exp_factor * significand;
      printf("Recomputed value formatted with %%.10E:\n");
      printf("\t(sign_factor * exp_factor * 0.shifted fraction)\n\n\t%.10E\n", recomputed_val);

      // Again, some pointer voodoo
      unsigned int* rip;
      rip = (unsigned int*) &recomputed_val;

      // Compare bit representations via pointer dereference
      if (*uip == *rip){
        printf("\nBit representations are identical\n");
      } else {
        printf("\nBit representations are different!!!\n");
      }

    }
  } else if (float_rep.exponent_val == 0xff) {
    if (float_rep.fraction_val == 0){
      // Infinite
      printf("Recomputed value (infinite): %cINF\n", sign_factor<0?'-':'+');
    } else {
      // NaN (not a number)
      printf("Recomputed value (not a number): %cNAN\n", sign_factor<0?'-':'+');
    }
  } else {
    short int ex = (float_rep.exponent_val-127);
    printf("  exponent (biased): %d\n", float_rep.exponent_val);
    printf("  exponent  (-127): %d\n", ex);

    // Compute the exponent by successive division by 2
    float exp_factor = shift_float32(1.0, ex);
    printf("    exponent factor: %.10E\n", exp_factor);

    // Compute the significand by successive division by 2
    float significand = 1.0 + shift_float32(1.0 * float_rep.fraction_val, -23);
    printf(" unshifted fraction: %.10E\n", 1.0*float_rep.fraction_val);
    printf(" 1.shifted fraction: %.10E\n", significand);

    float recomputed_val = 1.0 * sign_factor * exp_factor * significand;
    printf("Recomputed value formatted with %%.10E:\n");
    printf("\t(sign_factor * exp_factor * 1.shifted fraction)\n\n\t%.10E\n", recomputed_val);

    // Again, some pointer voodoo
    unsigned int* rip;
    rip = (unsigned int*) &recomputed_val;

    // Compare bit representations via pointer dereference
    if (*uip == *rip){
      printf("\nBit representations are identical\n");
    } else {
      printf("\nBit representations are different!!!\n");
    }
  }
  printf("\n");

  return EXIT_SUCCESS;
}

