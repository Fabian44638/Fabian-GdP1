{-# LANGUAGE EmptyDataDecls, RankNTypes, ScopedTypeVariables #-}

module CarryRippleDemo
where

import SignedIntegerAsBitList(SBList, show,
       fromBitList, mkFromInteger,
       asInteger, asUnsignedInteger, 
       getLimitsSignedInteger,
       one's, two's, add, sub, add_demo, sub_demo);


-- For a documentation see the slides in file 'WiesoDualsystem.pdf'
--
-- Source: 11/2023
-- ~/GdPI/Vorlesung/Dualsystem/WiesoDualsystem.odp
-- (C) FABR THI
