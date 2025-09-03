module Lib
    ( Calc(..)
    , calc
    ) where

data Calc a = Add a
            | Sub a
   deriving (Eq,Show)


-- Zero clue what ka and ks are supposed to be. Activity wasn't clear at all.
-- Somehow the implementation where I do nothing(?) gives me the most passed tests.
-- Really not sure what the problem is. Sigh.

-- unsignedAdd :: (Num a, Ord a) => a -> a -> a
-- unsignedAdd x y = if y < 0 then unsignedAdd x (y+1000000) else x + y

-- unsignedSub :: (Num a, Ord a) => a -> a -> a
-- unsignedSub x y = if x < y then unsignedSub (x+1000000) y else x - y

calc :: Num a => [Calc a] -> a -> (a -> a) -> (a -> a) -> a
calc xx init ka ks = aux init xx ka ks
    where aux a [] ka ks = ka (ks a)
          aux a ((Add i):xs) ka ks = aux (ka (ks (a + i))) xs ka ks
          aux a ((Sub i):xs) ka ks = aux (ka (ks (a - i))) xs ka ks

-- unsignedAdd :: (Num a, Ord a) => a -> a -> a
-- unsignedAdd x y = if y < 0 then unsignedAdd x (y+1000000) else x + y

-- unsignedSub :: (Num a, Ord a) => a -> a -> a
-- unsignedSub x y = if x < y then unsignedSub (x+1000000) y else x - y

-- calc :: (Num a, Ord a) => [Calc a] -> a -> (a -> a) -> (a -> a) -> a
-- calc xx init ka ks = aux init xx ka ks
--    where aux a [] ka ks = ka (ks (unsignedAdd 0 a))
--          aux a ((Add i):xs) ka ks = aux (ka (ks (unsignedAdd a i))) xs ka ks
--          aux a ((Sub i):xs) ka ks = aux (ka (ks (unsignedSub a i))) xs ka ks