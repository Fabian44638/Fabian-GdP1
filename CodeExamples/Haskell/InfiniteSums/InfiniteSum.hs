module InfiniteSum
where

-- Consider the infinite sequence of summands
-- (unendliche Folge der Summanden)
--
-- < 1 / (2^i) >  where i is a natural number
--

summands :: [Double]
summands = [ 1 / (2^i) | i <- [0..] ]

-- take 5 summands 
-- > [1.0,0.5,0.25,0.125,6.25e-2]

-- Partial sum (Partialsumme)

s_partial :: Int -> Double
s_partial n = sum (take (n+1) summands)

-- Infinite series (Unendliche Reihe) s = lim_{n -> inf}: s_partial n
-- 
-- In Haskell, we can define

s = sum summands

-- Die Berechnung von s terminiert natürlich nicht.
--
-- Aber:
--   * die Folge der Partialsummen s_partial konvergiert
--     --> Nachweis per Quotientenkriterium von d'Alembert
--     siehe: http://www.math-grain.de/download/m2/reihen/reihe-2.pdf
--
--   * man kann beweisen, dass für den Grenzwert s gilt: s = 2.0
--

-- Sequence of partial sums (Folge der Partialsummen)

s_partial_seq = [ s_partial n | n <- [0..] ]

{--
Empirischer 'Nachweis' (Achtung: wir haben nur doppelte Genauigkeit (Double)

take 100 s_partial_seq

>

[1.0
,1.5
,1.75
...
,2.0
,2.0]

--}

-- By induction, we can prove for n >= 0:
--
--    s_partial n = 2 -  1/2^n
--
-- and thus:
--
--    lim_{n -> inf}: s_partial n  = 2
--
-- Sketch:
--
-- s_partial n = 2 - 2 + s_partial n
--             = 2 - ( 2 - s_partial n)
--             = 2 - ( (2^(n+1) - ( 2^n + 2^(n-1) + ... + 2^0) ) / 2^n)
--            (*)
--             = 2 - ( (2^(n+1) - ( 2^(n+1) - 1) ) / 2^n)
--             = 2 - 1 / 2^n
--
-- (*) since by induction:
--
--     sum  [2^i  | i <- [0..n]] == 2^(n+1) - 1
--

s_partial2 n = 2 - (1 / (2^n))

{--

Empirischer 'Nachweis' (Achtung: wir haben nur doppelte Genauigkeit (Double)

and $ take 100  $ [s_partial n == s_partial2 n | n <- [0..] ]

>
True

--}

s_partial_seq2  = [ s_partial2 n | n <- [0..] ]

{--
take 100 s_partial_seq2

>

[1.0
,1.5
,1.75
...
,2.0
,2.0]

--}

