Question: does a switch statement produce faster machine code
          than an if_else cascade?

Answer: depends on the processor architecture and the compiler used.

On a processor with a rich instruction set a compiler with a good optimizer
probably produces the same machine code.

Examples: for processor i686 and gcc compiler (our 32bit VM),
          the code produced by optimizing levels O1 and better
          is identical for our example.

int do_selection(int d){
  int res;
  switch(d) {
    case MO ... FR:
      res=0xf1;
      break;
    case SA:
      res=0xf2;
      break;
    case SO:
      res=0xf3;
      break;
    default:
      res=0xf4;
  }
  return res;
}

versus

int do_selection(int d){
  int res;
  if (d ==MO || d==DI || d==MI || d==DO || d==FR) {
    res=0xf1;
  } else if (d == SA) {
    res=0xf2;
  } else if (d == SO) {
    res=0xf3;
  } else {
    res=0xf4;
  }
  return res;
}

#--- check yourself

make clean; make CFLAGS_ADD="-O0"

diff use_switch.disass use_if.disass

cat use_switch.disass

cat use_if.disass

# repeat for levels O1, O2 and O3

make clean; make CFLAGS_ADD="-O1"

make clean; make CFLAGS_ADD="-O2"

make clean; make CFLAGS_ADD="-O3"


