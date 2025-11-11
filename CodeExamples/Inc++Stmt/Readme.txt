A demo to answer the questions:

* does a standalone statement      i++
  do the same as the  statements   i = i+1  or i += 1

* does the standalone statement    i++
  execute faster than statements   i = i+1  or i += 1

* does a standalone statement      i++
  do the same as standalone        ++i

In the text above 'standalone statement i++' means:
   The expression i++ as a statement by itsself.

   e.g. 
   - in the assignment y = a[i++] the expression i++ is not a statment
   - in the for-loop header for (int i=0; i < 10; i++) the i++ is a statement
   - in the sequence  a=5; i++; b=10 the i++ is a statement

Description:

build: make clean; make

run:  bin/incV1
      bin/incV2

show assembler:

      cat incV1.c.disass
      cat incV2.c.disass

compare assembler code:

      diff incV1.c.disass incV2.c.disass


cleanup: make clean

