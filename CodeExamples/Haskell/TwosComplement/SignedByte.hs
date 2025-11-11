{-# LANGUAGE EmptyDataDecls, RankNTypes, ScopedTypeVariables #-}

-- Note: negativ numbers must be surrounded by (_)
-- (-3) `mod` 256  --> 253

module SignedByte
where

sbyte_modulus = 256

sbyte_max_val = (sbyte_modulus `div` 2) -  1     --   127
sbyte_min_val = (sbyte_modulus `div` 2) * (-1)   -- (-128)

data SByte = SB Int          deriving (Eq, Ord)

instance Show SByte where
   show (SB n) = "fromInt (" 
                  ++ show (if n > sbyte_max_val then n - sbyte_modulus else n) 
                  ++ ")"

-- Representation of integer values n in type SByte

fromInt :: Int -> SByte

fromInt n = if n < sbyte_min_val || sbyte_max_val < n
            then error ("function fromInt; Int value must be in range ["
                        ++ show sbyte_min_val ++ ".."
                        ++ show sbyte_max_val ++ "]")
            else SB (encode n)

            where
              encode m
                | m == 0    = 0
                | m <  0    = sbyte_modulus + m
                | otherwise = m


-- Test for the sign of values in type SByte

ispos :: SByte -> Bool
ispos (SB n) = n <= sbyte_max_val   --   0 .. 127 for sbyte_modulus == 256

isneg :: SByte -> Bool
isneg (SB n) = n >  sbyte_max_val   -- 128 .. 255 for sbyte_modulus == 256


-- One's complement

oneC :: SByte -> SByte
oneC (SB n) = SB ((sbyte_modulus - 1) - n)

-- Two's complement
twoC :: SByte -> SByte
twoC (SB n) = if n == 0
              then (SB 0)
              else (SB (sbyte_modulus - n))

-- Reference implementation of addition with check for overflow
-- See http://teaching.idallen.com/dat2343/10f/notes/040_overflow.txt

refadd :: Int -> Int -> Int
a `refadd` b
   | a + b < (sbyte_min_val) = error "overflow"
   | a + b > (sbyte_max_val) = error "overflow"
   | otherwise               = a + b

-- Reference implementation of subtraction with check for overflow
-- See http://teaching.idallen.com/dat2343/10f/notes/040_overflow.txt

refsub :: Int -> Int -> Int
a `refsub` b
   | a - b < sbyte_min_val = error "function refsub; overflow"
   | a - b > sbyte_max_val = error "function refsub; overflow"
   | otherwise             = a - b

-- Implementation of addition in type SByte
-- See http://teaching.idallen.com/dat2343/10f/notes/040_overflow.txt

-- Function raw-add is addition modulo sbyte_modulus 
-- without a post check for overflow

raw_add :: SByte -> SByte -> SByte
SB n `raw_add` SB m = SB ((n + m) `mod` sbyte_modulus)

add :: SByte -> SByte -> SByte
a `add` b
   | ispos a && ispos b && isneg thesum = error "function add; SB overflow"
   | isneg a && isneg b && ispos thesum = error "function add; SB overflow"
   | otherwise                           = thesum
   where
      thesum = a `raw_add` b

-- Implementation of subtraction in type SByte using two's complement
-- See http://teaching.idallen.com/dat2343/10f/notes/040_overflow.txt

sub :: SByte -> SByte -> SByte
a `sub` b
   | ispos a && isneg b && isneg thediff = error "function sub; SB overflow"
   | isneg a && ispos b && ispos thediff = error "function sub; SB overflow"
   | otherwise                           = thediff
   where
      thediff = a `raw_add` (twoC b) -- subtract b by adding two's complement of b





{-- Important relation between one's complement and two's complement

Note: (twoC a) == (oneC a) `add` (fromInt 1)

Proof:

We have: (fromInt 1) == (SB 1)

Consider the cases:
      a == (SB 0)
   or a == (SB n), where 0 < n < sbyte_modulus


case a == (SB 0):

   (oneC a) `add` (fromInt 1)
==
   SB ((sbyte_modulus - 1) - 0) `add` (SB 1)
==
   SB (sbyte_modulus - 1) `add` (SB 1)

== { operand 1 is negative and operand 2 is positive
     --> overflow is impossible due to 040_overflow.txt }
   
   SB (sbyte_modulus - 1) `raw_add` (SB 1)
== 
   SB (((sbyte_modulus - 1) + 1) `mod` sbyte_modulus)
==
   SB (sbyte_modulus `mod` sbyte_modulus)
==
   SB 0
==   
   twoC a

case a == (SB n), where 0 < n < sbyte_modulus:   

   (oneC a) `add` (fromInt 1)
==
   SB ((sbyte_modulus - 1) - n) `add` (SB 1)
== { 0 < n < sbyte_modulus:
     --> operand 1   (SB ((sbyte_modulus - 1) - n)) and
         the sum     (SB (sbyte_modulus - n))       have the same sign
         and operand 2 is positive
     --> overflow is impossible due to 040_overflow.txt }

   SB (sbyte_modulus - 1 - n) `raw_add` (SB 1)
== 
   SB (((sbyte_modulus - 1 -n ) + 1) `mod` sbyte_modulus)
==
   SB ((sbyte_modulus - n ) `mod` sbyte_modulus)
== { 0 < n < sbyte_modulus --> 0 < sbyte_modulus - n < sbyte_modulus } 
   SB (sbyte_modulus - n)
== 
   twoC a

--}

