// Investigate the IEEE 754 storage format of double precision floating point numbers
// See
// https://en.wikipedia.org/wiki/Single-precision_floating-point_format

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <math.h>       /* isnormal */

// A data type for modelling the bitwise representation of IEEE 754
// double values with 8 byte

struct doublerep {
  char byte1_rep[8+1];
  char byte2_rep[8+1];
  char byte3_rep[8+1];
  char byte4_rep[8+1];
  char byte5_rep[8+1];
  char byte6_rep[8+1];
  char byte7_rep[8+1];
  char byte8_rep[8+1];

  char all_rep[64+1];
  char sign_rep[1+1];
  char exponent_rep[11+1];
  char fraction_rep[52+1];

  unsigned char byte1_val;
  unsigned char byte2_val;
  unsigned char byte3_val;
  unsigned char byte4_val;
  unsigned char byte5_val;
  unsigned char byte6_val;
  unsigned char byte7_val;
  unsigned char byte8_val;

  unsigned char sign_val;
  unsigned short exponent_val;
  unsigned long long fraction_val;
};

// ---------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------

void make_bit_string(unsigned long long ui, char* buf, int pos){
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

struct doublerep make_bitrep64(unsigned long long ui){
  struct doublerep res;

  // create values by bit masking
  res.byte1_val  = (ui & 0xff00000000000000)>>56;
  res.byte2_val  = (ui & 0x00ff000000000000)>>48;
  res.byte3_val  = (ui & 0x0000ff0000000000)>>40;
  res.byte4_val  = (ui & 0x000000ff00000000)>>32;
  res.byte5_val  = (ui & 0x00000000ff000000)>>24;
  res.byte6_val  = (ui & 0x0000000000ff0000)>>16;
  res.byte7_val  = (ui & 0x000000000000ff00)>>8;
  res.byte8_val  = (ui & 0x00000000000000ff);

  res.sign_val     = (ui & 0x8000000000000000)>>63;
  res.exponent_val = (ui & 0x7FF0000000000000)>>52;
  res.fraction_val = (ui & 0x000FFFFFFFFFFFFF);

  make_bit_string(res.byte1_val, res.byte1_rep, 8);
  make_bit_string(res.byte2_val, res.byte2_rep, 8);
  make_bit_string(res.byte3_val, res.byte3_rep, 8);
  make_bit_string(res.byte4_val, res.byte4_rep, 8);
  make_bit_string(res.byte5_val, res.byte5_rep, 8);
  make_bit_string(res.byte6_val, res.byte6_rep, 8);
  make_bit_string(res.byte7_val, res.byte7_rep, 8);
  make_bit_string(res.byte8_val, res.byte8_rep, 8);

  make_bit_string(ui, res.all_rep, 64);
  make_bit_string(res.sign_val, res.sign_rep, 1);
  make_bit_string(res.exponent_val, res.exponent_rep, 11);
  make_bit_string(res.fraction_val, res.fraction_rep, 52);

  return res;
}

double shift_float64(double val, short int ex){
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

bool make_float_from_hex64(char* str, double *fp){
  // We expect something like this as str: c115|1959|a000|0000

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

  if (nibble_cnt != 16){
    fprintf(stderr,"Hexadecimal value must contain exactly 16 nibbles\n");
    return false;
  }

  // We are at the '\0' of str.
  if (p != str) p--;

  // Compile the hex value from the nibble string ignoring '|' on the way back
  unsigned long long ui = 0;
  unsigned shift = 0;
  while(str <= p){
    if (*p != '|'){
      switch(*p){
        case '0':
          break;
        case '1':
          ui += 1* (1ULL<<shift);
          break;
        case '2':
          ui += 2* (1ULL<<shift);
          break;
        case '3':
          ui += 3* (1ULL<<shift);
          break;
        case '4':
          ui += 4* (1ULL<<shift);
          break;
        case '5':
          ui += 5* (1ULL<<shift);
          break;
        case '6':
          ui += 6* (1ULL<<shift);
          break;
        case '7':
          ui += 7* (1ULL<<shift);
          break;
        case '8':
          ui += 8* (1ULL<<shift);
          break;
        case '9':
          ui += 9* (1ULL<<shift);
          break;
        case 'a':
        case 'A':
          ui += 10* (1ULL<<shift);
          break;
        case 'b':
        case 'B':
          ui += 11* (1ULL<<shift);
          break;
        case 'c':
        case 'C':
          ui += 12* (1ULL<<shift);
          break;
        case 'd':
        case 'D':
          ui += 13* (1ULL<<shift);
          break;
        case 'e':
        case 'E':
          ui += 14* (1ULL<<shift);
          break;
        case 'f':
        case 'F':
          ui += 15* (1ULL<<shift);
          break;
      }
      shift += 4;
    }
    p--;
  }

  // Assign the bitpattern of the compiled hexvalue to fp
  unsigned long long* uip;
  uip = (unsigned long long*) fp;
  *uip = ui; 
  return true;
}

bool make_float_from_bin64(char* str, double *fp){
  // We expect something like this as str
  // 1|10000010001|0101000110010101100110100000000000000000000000000000

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

  if (bit_cnt != 64){
    fprintf(stderr,"Binary value must contain exactly 64 bits\n");
    return false;
  }

  // We are at the '\0' of str.
  if (p != str) p--;

  // Compile the binary value from the binary string ignoring '|' on the way back
  unsigned long long ui = 0;
  unsigned shift = 0;
  while(str <= p){
    if (*p != '|'){
      switch(*p){
        case '0':
          break;
        case '1':
          ui += 1* (1ULL<<shift);
          break;
      }
      shift += 1;
    }
    p--;
  }

  // Assign the bitpattern of the compiled hexvalue to fp
  unsigned long long* uip;
  uip = (unsigned long long*) fp;
  *uip = ui; 
  return true;
}

void usage(char* prog){
  printf("\nUsage:\n");
  printf("%s mode values\n", prog);
  printf("\nwhere mode is one of: -b, -f, -x \n");
  printf("\nExamples for mode -f: 64bit float\n");
  printf("%s -f -345686.406250\n", prog);
  printf("%s -f -3.4568640625E+05\n", prog);
  printf("\nExamples for mode -b: 64bit binary (bit separators '|' are optional) \n");
  printf("%s -b '1|10000010001|0101000110010101100110100000000000000000000000000000'\n", prog);
  printf("\nExamples for mode -x: 64bit hexadecimal (nibble separators '|' are optional) \n");
  printf("%s -x 'C11|51959a0000000'      some normal case\n", prog);
  printf("%s -x '001|0000000000000'      some normal case\n", prog);
  printf("%s -x '7FE|FFFFFFFFFFFFF'      some normal case\n", prog);
  printf("%s -x '000|0000000000000'      non-normal case +zero\n", prog);
  printf("%s -x '800|0000000000000'      non-normal case -zero\n", prog);
  printf("%s -x '000|0000000000001'      non-normal case +subnormal\n", prog);
  printf("%s -x '000|FFFFFFFFFFFFF'      non-normal case +subnormal\n", prog);
  printf("%s -x '800|0000000000001'      non-normal case -subnormal\n", prog);
  printf("%s -x '800|FFFFFFFFFFFFF'      non-normal case -subnormal\n", prog);
  printf("%s -x '7FF|0000000000000'      non-normal case +INF\n", prog);
  printf("%s -x 'FFF|0000000000000'      non-normal case -INF\n", prog);
  printf("%s -x '7FF|FFFFFFFFFFFFF'      non-normal case +NAN\n", prog);
  printf("%s -x 'FFF|FFFFFFFFFFFFF'      non-normal case -NAN\n", prog);
  printf("\n");
}

// ---------------------------------------------------------------------
// MAIN
// ---------------------------------------------------------------------

int main(int argc, char **argv) {
  // Check our assumptions about the storage format
  assert(sizeof(double) == 8);
  assert(sizeof(unsigned long long) == 8);

  // Variable for storing the 64bit float
  double float_val;

  // Process command line (simple)
  if (argc == 1 || argc > 3){
    usage(argv[0]);
    return EXIT_FAILURE;
  }

  if (strcmp(argv[1],"-f") == 0){
    if(1 != sscanf(argv[2], "%lf", &float_val)){
      fprintf(stderr,"Please provide a properly formatted floating point value\n");
      usage(argv[0]);
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1],"-x") == 0){
    if(! make_float_from_hex64(argv[2], &float_val)){
      fprintf(stderr,"Please provide a properly formatted hexadecimal string\n");
      usage(argv[0]);
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1],"-b") == 0){
    if(! make_float_from_bin64(argv[2], &float_val)){
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
  //  unsigned long long ui = float_val;
  // enforces a type conversion, where bits are not preserved
  unsigned long long* uip;
  uip = (unsigned long long*) &float_val;

  // Now, the integer pointer uip points to the variable containing the 
  // 8 byte double variable float_val

  printf("\n");
  if (isnormal(float_val)){
    printf("Double precision float variable formatted with %%.20E (normal):\n\n\t%.20E\n", float_val);
  } else {
    printf("Double precision float variable formatted with %%.20E (non-normal):\n\t%.20E\n", float_val);
  }
  printf("\n");
  printf("Hexadecimal representation in memory (msb first): %016llx\n", *uip);

  // Create a string representation of the binary value
  struct doublerep float_rep = make_bitrep64(*uip);

  printf("\n");
  printf("byte representation:\n");
  printf("   all: %64s\n" , float_rep.all_rep);
  printf("byte 1: %8s\n"  , float_rep.byte1_rep);
  printf("byte 2: %16s\n" , float_rep.byte2_rep);
  printf("byte 3: %24s\n" , float_rep.byte3_rep);
  printf("byte 4: %32s\n" , float_rep.byte4_rep);
  printf("byte 5: %40s\n" , float_rep.byte5_rep);
  printf("byte 6: %48s\n" , float_rep.byte6_rep);
  printf("byte 7: %56s\n" , float_rep.byte6_rep);
  printf("byte 8: %64s\n" , float_rep.byte8_rep);

  // Create a string representation of the IEEE 754 double value
  printf("\n");
  printf("IEEE 754 double components:\n");
  printf("     all: %64s\n", float_rep.all_rep);
  printf("    sign: %1s\n" , float_rep.sign_rep);
  printf("exponent: %12s\n", float_rep.exponent_rep);
  printf("fraction: %64s\n", float_rep.fraction_rep);

  // Recompute the value from the components
  printf("\n");
  printf("Re-computing the IEEE 754 double from its components:\n");

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
      short int ex = (-1022);
      printf("  exponent (   biased): %d\n", float_rep.exponent_val);
      printf("  exponent (subnormal): %d\n", ex);

      // Compute the exponent by successive division by 2
      double exp_factor = shift_float64(1.0, ex);
      printf("    exponent factor: %.20E\n", exp_factor);

      // Compute the significand by successive division by 2
      double significand = 0.0 + shift_float64(1.0 * float_rep.fraction_val, -52);
      printf(" unshifted fraction: %.20E\n", 1.0*float_rep.fraction_val);
      printf(" 0.shifted fraction: %.20E\n", significand);


      double recomputed_val = 1.0 * sign_factor * exp_factor * significand;
      printf("Recomputed value formatted with %%.20E:\n");
      printf("\t(sign_factor * exp_factor * 0.shifted fraction)\n\n\t%.20E\n", recomputed_val);

      // Again, some pointer voodoo
      unsigned long long* rip;
      rip = (unsigned long long*) &recomputed_val;

      // Compare bit representations via pointer dereference
      if (*uip == *rip){
        printf("\nBit representations are identical\n");
      } else {
        printf("\nBit representations are different!!!\n");
      }

    }
  } else if (float_rep.exponent_val == 0x7ff) {
    if (float_rep.fraction_val == 0){
      // Infinite
      printf("Recomputed value (infinite): %cINF\n", sign_factor<0?'-':'+');
    } else {
      // NaN (not a number)
      printf("Recomputed value (not a number): %cNAN\n", sign_factor<0?'-':'+');
    }
  } else {
    short int ex = (float_rep.exponent_val-1023);
    printf("  exponent (biased): %d\n", float_rep.exponent_val);
    printf("  exponent  (-1023): %d\n", ex);

    // Compute the exponent by successive division by 2
    double exp_factor = shift_float64(1.0, ex);
    printf("    exponent factor: %.20E\n", exp_factor);

    // Compute the significand by successive division by 2
    double significand = 1.0 + shift_float64(1.0 * float_rep.fraction_val, -52);
    printf(" unshifted fraction: %.20E\n", 1.0*float_rep.fraction_val);
    printf(" 1.shifted fraction: %.20E\n", significand);

    double recomputed_val = 1.0 * sign_factor * exp_factor * significand;
    printf("Recomputed value formatted with %%.20E:\n");
    printf("\t(sign_factor * exp_factor * 1.shifted fraction)\n\n\t%.20E\n", recomputed_val);

    // Again, some pointer voodoo
    unsigned long long* rip;
    rip = (unsigned long long*) &recomputed_val;

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

