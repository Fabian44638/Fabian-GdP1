{-# LANGUAGE EmptyDataDecls, RankNTypes, ScopedTypeVariables #-}

module SignedIntegerAsBitList(SBList, show,
       fromBitList, mkFromInteger,
       asInteger, asUnsignedInteger,
       getLimitsSignedInteger,
       one's, two's, add, sub, add_demo, sub_demo)
where

import Data.List
import Data.Foldable

-- We hide the constructor and implement our own instance for show.
data SBList = SBL [Char] deriving (Eq, Ord)

instance Show SBList
   where
   --show (SBL ls) = "fromBitList " ++ show (length ls) ++ " " ++ show ls
   show (SBL ls)
     | len <= 8          = "fromBitList " ++ show (length ls) ++ " " ++ show ls
     | len `mod` 8 /= 0  = "fromBitList " ++ show (length ls)
                           ++ " \"" ++  (take (len `mod` 8) ls)
                           ++ show_aux (drop (len `mod` 8) ls)
                           ++ "\""
     | otherwise         = "fromBitList " ++ show (length ls)
                           ++ " \"" ++  (take 8 ls)
                           ++ show_aux (drop 8 ls)
                           ++ "\""
     where
       len = length ls
       show_aux [] = []
       show_aux xs = "." ++ (take 8 xs) ++ show_aux (drop 8 xs)

-- Representation of bit list ('O'|'L'|'.')* in type SBList

zeropad       :: Int -> [Char]  -> SBList
zeropad numbits ls
  | len <  numbits = SBL $ (replicate (numbits - len) 'O') ++ ls
  | len == numbits = SBL ls
  | otherwise      = error "Function zeropad: argument has too much bits"
  where len = length ls

fromBitList      :: Int -> [Char]  -> SBList
fromBitList numbits cs = zeropad numbits (fromBitList_aux cs)
  where
    fromBitList_aux [] = []
    fromBitList_aux ('O':cs) = 'O' : fromBitList_aux cs
    fromBitList_aux ('L':cs) = 'L' : fromBitList_aux cs
    fromBitList_aux ('.':cs) = fromBitList_aux cs
    fromBitList_aux (c:cs  ) = error $ "Function fromBitList:"
                                    ++ " invalid character '"
                                    ++ [c] ++ "'"


-- Representation of Integer values n in type SBList
mkFromInteger :: Int -> Integer -> SBList

mkFromInteger numbits ival
  | ival < min_val  = error $ "Function mkFromInteger: value "
                              ++ show ival
                              ++ " is too small for signed integer with "
                              ++ show numbits ++ " bits"

  | ival > max_val  = error $ "Function mkFromInteger: value "
                              ++ show ival
                              ++ " is too large for signed integer with "
                              ++ show numbits ++ " bits"
  | otherwise       = zeropad numbits 
                              (encodeToBitList
                                 (representAsTwo'sComplement ival) [])
  where
    modulus = 2^numbits
    max_val = (modulus `div` 2) -  1
    min_val = (modulus `div` 2) * (-1)

    representAsTwo'sComplement i
      | i == 0    = 0
      | i <  0    = modulus + i
      | otherwise = i

    encodeToBitList val acc
      | val == 0   = 'O' : acc
      | val == 1   = 'L' : acc
      | otherwise  = let bit  = val `mod` 2
                         rest = val `div` 2
                     in  if bit == 0
                         then encodeToBitList rest ('O' : acc)
                         else encodeToBitList rest ('L' : acc)

-- Conversion of values of type SBList back to Integer values
asInteger :: SBList -> Integer

asInteger (SBL ls)
  | ival > max_val  = ival - modulus
  | otherwise       = ival
  where
    modulus = 2^(length ls)
    max_val = (modulus `div` 2) -  1
    min_val = (modulus `div` 2) * (-1)

    ival = foldl' (\a c -> if c == 'L' then 2*a + 1 else 2*a) 0 ls

-- Show unsigned integer value of SBList (the unsigend value of two's complement encoding)
asUnsignedInteger :: SBList -> Integer

asUnsignedInteger (SBL ls) =
  foldl' (\a c -> if c == 'L' then 2*a + 1 else 2*a) 0 ls

-- Show limits for Integer values encoded as signed integer with numbits
getLimitsSignedInteger:: Int -> String
getLimitsSignedInteger numbits =
  "modulus: " ++ show modulus
     ++ "; min_val: " ++ show min_val
     ++ "; max_val: " ++ show max_val
  where
    modulus = 2^numbits
    max_val = (modulus `div` 2) -  1
    min_val = (modulus `div` 2) * (-1)

-- Tests of sign bit

ispos :: SBList -> Bool
ispos (SBL bs) = head bs == 'O'

isneg :: SBList -> Bool
isneg (SBL bs) = head bs == 'L'

-- ------------------------------------------------------------
-- Some helper functions
-- ------------------------------------------------------------
-- Only for internal use

-- Strip constructor SBL; extract the list of bits

stripSBL :: SBList -> [Char]
stripSBL (SBL ls) = ls

rightjust :: Int -> [Char] -> [Char]
rightjust w s    = replicate (w - (length s)) ' ' ++ s

-- ----------------------------------------------------------------------
-- Implementation of binary arithmetics on type SBList
-- ----------------------------------------------------------------------

-- Logical gates

not_gate  :: Char -> Char
not_gate  'O' = 'L'
not_gate  'L' = 'O'
not_gate   _  = error "not_gate: invalid input"

and_gate  :: Char -> Char -> Char
and_gate  'O' 'O' = 'O'
and_gate  'O' 'L' = 'O'
and_gate  'L' 'O' = 'O'
and_gate  'L' 'L' = 'L'
and_gate   _   _  = error "and_gate: invalid input"

or_gate   :: Char -> Char -> Char
or_gate  'O' 'O' = 'O'
or_gate  'O' 'L' = 'L'
or_gate  'L' 'O' = 'L'
or_gate  'L' 'L' = 'L'
or_gate   _   _  = error "or_gate: invalid input"

xor_gate  :: Char -> Char -> Char
xor_gate  'O' 'O' = 'O'
xor_gate  'O' 'L' = 'L'
xor_gate  'L' 'O' = 'L'
xor_gate  'L' 'L' = 'O'
xor_gate   _   _  = error "xor_gate: invalid input"

-- Half adder for a single bit implemented by an and gate and an xor gate
-- See e.g. https://de.wikipedia.org/wiki/Halbaddierer
--
-- Inputs:
--     a: the bit of first  addend
--     b: the bit of second addend
--
-- Outputs:
--  cout: the carry out bit
--     s: the sum of bits a and b

half_adder :: Char -> Char -> (Char, Char)

half_adder a b = 
  let cout = and_gate a b
      s    = xor_gate a b
  in (cout, s)

--
-- Full adder for a single bit implemented by two half adders and an or-gate
-- See e.g. https://de.wikipedia.org/wiki/Volladdierer
--
-- Inputs:
--     a: the bit of first  addend
--     b: the bit of second addend
--   cin: the carry in
--
-- Outputs:
--  cout: the carry out bit
--     s: the sum of bits a and b

full_adder :: Char -> Char -> Char -> (Char, Char)

full_adder a b cin = 
  let (ha1_cout, ha1_s) = half_adder a b
      (ha2_cout, ha2_s) = half_adder ha1_s cin
      cout              = or_gate ha1_cout ha2_cout
      s                 = ha2_s
  in (cout, s)

--
-- One's complement (inversion of bits)
--
one's:: SBList -> SBList

one's (SBL bs) = SBL (map (\b -> if b == 'L' then 'O' else 'L') bs)

--
-- Two's complement: fast implementation
--
-- From rightmost bit (LSB) to the leftmost bit (MSB)
-- * copy all O bits upto the first L bit
-- * copy the first L bit
-- * invert all bits left from the first L bit

two's:: SBList -> SBList

two's (SBL bs) = SBL (two's_aux (reverse bs) False [])
  where
  two's_aux [] inv acc = acc
  two's_aux ('O':bs) False acc = two's_aux bs False ('O':acc)
  two's_aux ('L':bs) False acc = two's_aux bs True  ('L':acc)
  two's_aux bs       True  acc =
     reverse (map (\b -> if b == 'L' then 'O' else 'L') bs) ++ acc

-- ----------------------------------------------------------------------
-- Carry Ripple Adder for n bits without inverter.
-- ----------------------------------------------------------------------
-- This version cannot be used directly for subtraction.
-- We need to compute the two's complement of the second argument before.
-- The ripple adder does not care about overflow detection.
--
-- The overflow detection is done by functions add and sub or
-- their variants add_demo and sub_demo.

-- ----------------------------------------------------
-- This is a version of the ripple adder implemented by
-- an explicit recursion.

carry_ripple_exp_rec :: SBList -> SBList -> (Char, SBList)
carry_ripple_exp_rec (SBL as) (SBL bs) =
  let (cout, ss) = carry_ripple_aux full_adder
                     'O' [] (reverse as) (reverse bs)
  in if length as == length bs
     then (cout, SBL ss)
     else error "Function carry_ripple: unequal length of arguments"
  where
    carry_ripple_aux _ cin acc []     []     = (cin, acc)
    carry_ripple_aux f cin acc (a:as) (b:bs) =
      let (co, so) = f a b cin
      in carry_ripple_aux f co (so:acc) as bs

-- ----------------------------------------------------
-- This is a version of the ripple adder implemented by
-- the folding operator foldl'

-- The function full_adder_adaptor wraps the standard full_adder.
-- This adaptor can be used as the function passed to foldl', which
-- needs to transport the current carry and the accumulated sum bits
-- as a single argument.

carry_ripple :: SBList -> SBList -> (Char, SBList)
carry_ripple (SBL as) (SBL bs) =
  if length as == length bs
  then (cout, SBL ss)
  else error "Function carry_ripple: unequal length of arguments"

  where
    (cout, ss) = foldl' full_adder_adaptor ('O',[]) (zip (reverse as) (reverse bs))

    full_adder_adaptor :: (Char, [Char]) -> (Char, Char) -> (Char, [Char])
    full_adder_adaptor (ci, acc_ss) (a,b) =
      let (co, s) = full_adder a b ci
      in (co, s:acc_ss)


-- ----------------------------------------------------------------------
-- Addition of signed integers coded as SBLists with overflow detection.
-- ----------------------------------------------------------------------
-- See http://teaching.idallen.com/dat2343/10f/notes/040_overflow.txt
-- for background information about overflow detection.
add :: SBList -> SBList -> SBList
add a b
   | overflow  = error "Function add; SBL overflow"
   | otherwise = thesum  -- ignore the carry out!
   where
      (cout, thesum) = carry_ripple a b
      overflow       = ((ispos a && ispos b && isneg thesum) ||
                        (isneg a && isneg b && ispos thesum))


-- ----------------------------------------------------------------------
-- Subtraction of signed integers coded as SBLists with overflow detection.
-- ----------------------------------------------------------------------
-- We explicitly compute the two's complement of the second argument to sub.
--
-- See http://teaching.idallen.com/dat2343/10f/notes/040_overflow.txt
-- for background information about overflow detection.
sub :: SBList -> SBList -> SBList
sub a b
   | overflow  = error "Function sub; SBL overflow"
   | otherwise = thesum  -- ignore the carry out!
   where
      (cout, thesum) = carry_ripple a (two's b) -- use two's complement
      overflow       = ((ispos a && isneg b && isneg thesum) ||
                        (isneg a && ispos b && ispos thesum))

-- -----------------------------------------------------------------------
-- Special versions of addition and subtraction for demo purposes
-- -----------------------------------------------------------------------

-- Addition of signed integers coded as SBLists with overflow detection.
-- See http://teaching.idallen.com/dat2343/10f/notes/040_overflow.txt
-- for background information about overflow detection.
--
-- Demonstrate the computation

add_demo :: SBList -> SBList -> IO ()
add_demo a b =
   let as       = stripSBL a
       bs       = stripSBL b
       ss       = stripSBL thesum
       
       ia_string       = show (asInteger a)
       ib_string       = show (asInteger b)
       ithesum_string  = show (asInteger thesum)

       uia_string      = show (asUnsignedInteger a)
       uib_string      = show (asUnsignedInteger b)
       uithesum_string = show (asUnsignedInteger thesum)

       iwidth = maximum
                 (map length [ia_string, ib_string, ithesum_string ])

       uiwidth = maximum
                 (map length [uia_string, uib_string, uithesum_string ])
   in  putStrLn $
         -- Pretty print with demo output
         "\nAddition explained:" ++
         "\n  Integer numbers without parenthesis show the signed integer encoding." ++
         "\n  Integer numbers within  parenthesis show the original integer value." ++
         "\n    " ++
         "\n    " ++ as       ++ "   " ++ rightjust uiwidth uia_string
         ++ "  (" ++ rightjust iwidth ia_string ++ ")" ++
         "\n    " ++ bs ++ "   " ++ rightjust uiwidth uib_string
         ++ "  (" ++ rightjust iwidth ib_string ++ ")" ++
         "\n(+) " ++ replicate (length as) '-' ++
         "\n    " ++ ss ++ "   " ++ rightjust uiwidth uithesum_string
         ++ "  (" ++ rightjust iwidth ithesum_string ++ ")" ++
         "\n" ++
         "\n   Flags:" ++
         "\n     " ++ (if cout == 'L' then "Carry out (ignore)"   else "") ++
         "\n     " ++ (if overflow    then "Arithmetic overflow!" else "") ++
         "\n"
   where
      (cout, thesum) = carry_ripple a b
      overflow       = ((ispos a && ispos b && isneg thesum) ||
                        (isneg a && isneg b && ispos thesum))

-- Subtraction of signed integers coded as SBLists with overflow detection.
-- We explicitly compute the two's complement of the second argument to sub.
--
-- See http://teaching.idallen.com/dat2343/10f/notes/040_overflow.txt
-- for background information about overflow detection.
--
-- Demonstrate the computation

sub_demo :: SBList -> SBList -> IO ()
sub_demo a b =
   -- Some definitions for pretty printing
   let as       = stripSBL a
       bs       = stripSBL b
       ss       = stripSBL thesum
       two's_bs = stripSBL (two's b)
       
       ia_string       = show (asInteger a)
       ib_string       = show (asInteger b)
       itb_string      = show (asInteger (two's b))
       ithesum_string  = show (asInteger thesum)

       uia_string      = show (asUnsignedInteger a)
       uib_string      = show (asUnsignedInteger b)
       uitb_string     = show (asUnsignedInteger (two's b))
       uithesum_string = show (asUnsignedInteger thesum)

       iwidth = maximum
                 (map length [ia_string, ib_string, itb_string, ithesum_string ])

       uiwidth = maximum
                 (map length [uia_string, uib_string, uitb_string, uithesum_string ])
   in putStrLn $
         -- Pretty print with demo output
         "\nSubtraction explained:" ++
         "\n  Integer numbers without parenthesis show the signed integer encoding." ++
         "\n  Integer numbers within  parenthesis show the original integer value." ++
         "\n    " ++
         "\n    " ++ as       ++ "   the first  operand  : "
         ++ rightjust uiwidth uia_string ++ "  (" ++ rightjust iwidth ia_string ++ ")" ++
         "\n    " ++ bs       ++ "   the second operand  : "
         ++ rightjust uiwidth uib_string ++ "  (" ++ rightjust iwidth ib_string ++ ")" ++
         "\n    " ++ two's_bs ++ "   its two's complement: "
         ++ rightjust uiwidth uitb_string ++ "  (" ++ rightjust iwidth itb_string ++ ")" ++
         "\n    " ++
         "\n    " ++ as       ++ "   " ++ rightjust uiwidth uia_string
         ++ "  (" ++ rightjust iwidth ia_string ++ ")" ++
         "\n    " ++ two's_bs ++ "   " ++ rightjust uiwidth uitb_string
         ++ "  (" ++ rightjust iwidth itb_string ++ ")" ++
         "\n(+) " ++ replicate (length as) '-' ++
         "\n    " ++ ss ++ "   " ++ rightjust uiwidth uithesum_string
         ++ "  (" ++ rightjust iwidth ithesum_string ++ ")" ++
         "\n" ++
         "\n   Flags:" ++
         "\n     " ++ (if cout == 'L' then "Carry out (ignore)"   else "") ++
         "\n     " ++ (if overflow    then "Arithmetic overflow!" else "") ++
         "\n"
   where
      -- This is the computation for the subtraction
      (cout, thesum) = carry_ripple a (two's b) -- use two's complement
      overflow       = ((ispos a && isneg b && isneg thesum) ||
                        (isneg a && ispos b && ispos thesum))


