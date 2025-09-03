main :: IO ()    -- This says that main is an IO action.
main = return () -- This tells main to do nothing.

true :: p1 -> p2 -> p1
true = \ a b -> a
false :: p1 -> p2 -> p2
false = \ a b -> b
showb f = f True False