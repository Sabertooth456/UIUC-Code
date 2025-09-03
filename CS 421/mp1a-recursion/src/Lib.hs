--- Getting Started
--- ===============

--- Relevant Files
--- --------------

module Lib where

-- This line imports the Prelude module without certain functions
import Prelude hiding ( take, drop, reverse
                      , zip, zipWith
                      , map, foldl, foldr
                      , iterate, repeat
                      , replicate, cycle
                      , (++)
                      )
-- When you are allowed to use builtin functions Prepend them with "P."
-- for example `P.take`
import qualified Prelude as P

--- Problems
--- ========

--- Recursion
--- ---------

--- ### mytake

-- don't forget to put the type declaration or you will lose points!
mytake :: Int -> [a] -> [a]
mytake a [] = []
mytake a (x:xs) = if a <= 0 then []
    else x : mytake (a-1) xs

--- ### mydrop

-- don't forget to put the type declaration or you will lose points!
mydrop :: Int -> [a] -> [a]
mydrop a [] = []
mydrop a (x:xs) = if a <= 0 then x:xs
    else mydrop (a-1) xs

--- ### rev

-- don't forget to put the type declaration or you will lose points
rev :: [a] -> [a]
rev x = revA x []
revA :: [a] -> [a] -> [a]
revA [] acc = acc
revA (x:xs) acc = revA xs (x:acc)

--- ### app

-- don't forget to put the type declaration or you will lose points!
app :: [a] -> [a] -> [a]
app [] [] = []
app [] y = y
app x [] = x
app (x:xs) y = if null xs then x:y
    else x : app xs y

--- ### inclist

-- don't forget to put the type declaration or you will lose points!
inclist :: Num a => [a] -> [a]
inclist [] = []
inclist (x:xs) = (x+1) : inclist xs

--- ### sumlist

-- don't forget to put the type declaration or you will lose points!
sumlist :: Num a => [a] -> a
sumlist [] = 0
sumlist (x:xs) = x + sumlist xs

--- ### myzip

-- don't forget to put the type declaration or you will lose points!
myzip :: [a] -> [b] -> [(a, b)]
myzip [] [] = []
myzip [] b = []
myzip a [] = []
myzip (a:as) (b:bs) = (a, b) : myzip as bs

--- ### addpairs

-- don't forget to put the type declaration or you will lose points!
addpairs :: Num a => [a] -> [a] -> [a]
addpairs a b = addzipped (myzip a b)
addzipped :: Num a => [(a, a)] -> [a]
addzipped [] = []
addzipped ((a, b):abs) = (a+b) : addzipped abs

--- ### ones

-- don't forget to put the type declaration or you will lose points!
ones :: [Integer]
ones = [1,1..]

--- ### nats

-- don't forget to put the type declaration or you will lose points!
nats :: [Integer]
nats = [0,1..]

--- ### fib

-- don't forget to put the type declaration or you will lose points!
fib :: [Integer]
fib = fibAdd 0 1
fibAdd :: Integer -> Integer -> [Integer]
fibAdd a b = a : fibAdd b (a+b)

--- Set Theory
--- ----------

--- ### add

-- don't forget to put the type declaration or you will lose points!
add :: Ord a => a -> [a] -> [a]
add a [] = [a]
add a (x:xs)
    | a == x = x:xs
    | a < x = a:x:xs
    | a > x = x : add a xs

--- ### union

-- don't forget to put the type declaration or you will lose points!
union :: Ord a => [a] -> [a] -> [a]
union [] [] = []
union x [] = x
union [] y = y
union (x:xs) (y:ys)
    | x == y = x : union xs ys
    | x < y = x : union xs (y:ys)
    | otherwise = y : union (x:xs) ys

--- ### intersect

-- don't forget to put the type declaration or you will lose points!
intersect :: Ord a => [a] -> [a] -> [a]
intersect [] [] = []
intersect x [] = []
intersect [] y = []
intersect (x:xs) (y:ys)
    | x == y = x : intersect xs ys
    | x < y = intersect xs (y:ys)
    | otherwise = intersect (x:xs) ys

--- ### powerset

-- don't forget to put the type declaration or you will lose points!
powerset :: Ord a => [a] -> [[a]]
powerset [] = [[]]
powerset (x:xs) = union (powerset xs) (poweradd x (powerset xs))
poweradd :: Ord a => a -> [[a]] -> [[a]]
poweradd a [] = [[a]]
poweradd a [[]] = [[a]]
poweradd a (x:xs) = union [add a x] (poweradd a xs)
 -- https://en.wikipedia.org/wiki/Power_set#Recursive_definition

--- Higher Order Functions
--- ----------------------

--- ### inclist'

-- don't forget to put the type declaration or you will lose points!
inclist' :: Num a => [a] -> [a]
inclist' x = mymap (\y -> y + 1) x
mymap :: (a -> b) -> [a] -> [b]
mymap f [] = []
mymap f (x:xs) = f x : mymap f xs
 -- https://stackoverflow.com/questions/24612952/a-simple-version-of-haskells-map
 -- Couldn't figure out how to write it without using a recursive map function

--- ### sumlist'

-- don't forget to put the type declaration or you will lose points!
sumlist' :: (Num a) => [a] -> a
sumlist' x = myfoldl (+) x 0
myfoldl :: Num a => (a -> a -> a) -> [a] -> a -> a
myfoldl f [] k = k
myfoldl f (x:xs) k = f x (myfoldl f xs k)
 -- Not sure if works, or just janky lol