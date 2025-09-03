module Lib where
import Data.HashMap.Strict as H (HashMap, empty, fromList, insert, lookup, union)
import Data.Maybe (isNothing, fromJust)
import Data.Bool (Bool(False, True))


--- Data Types
--- ----------

--- ### Environments and Results

type Env  = H.HashMap String Val
type PEnv = H.HashMap String Stmt

type Result = (String, PEnv, Env)

--- ### Values

data Val = IntVal Int
         | BoolVal Bool
         | CloVal [String] Exp Env
         | ExnVal String
    deriving (Eq)

instance Show Val where
    show (IntVal i) = show i
    show (BoolVal i) = show i
    show (CloVal xs body env) = "<" ++ show xs   ++ ", "
                                    ++ show body ++ ", "
                                    ++ show env  ++ ">"
    show (ExnVal s) = "exn: " ++ s

--- ### Expressions

data Exp = IntExp Int
         | BoolExp Bool
         | FunExp [String] Exp
         | LetExp [(String,Exp)] Exp
         | AppExp Exp [Exp]
         | IfExp Exp Exp Exp
         | IntOpExp String Exp Exp
         | BoolOpExp String Exp Exp
         | CompOpExp String Exp Exp
         | VarExp String
    deriving (Show, Eq)

--- ### Statements

data Stmt = SetStmt String Exp
          | PrintStmt Exp
          | QuitStmt
          | IfStmt Exp Stmt Stmt
          | ProcedureStmt String [String] Stmt
          | CallStmt String [Exp]
          | SeqStmt [Stmt]
    deriving (Show, Eq)

--- Primitive Functions
--- -------------------

intOps :: H.HashMap String (Int -> Int -> Int)
intOps = H.fromList [ ("+", (+))
                    , ("-", (-))
                    , ("*", (*))
                    , ("/", (div))
                    ]

boolOps :: H.HashMap String (Bool -> Bool -> Bool)
boolOps = H.fromList [ ("and", (&&))
                     , ("or", (||))
                     ]

compOps :: H.HashMap String (Int -> Int -> Bool)
compOps = H.fromList [ ("<", (<))
                     , (">", (>))
                     , ("<=", (<=))
                     , (">=", (>=))
                     , ("/=", (/=))
                     , ("==", (==))
                     ]

--- Problems
--- ========

--- Lifting Functions
--- -----------------

liftIntOp :: (Int -> Int -> Int) -> Val -> Val -> Val
liftIntOp op (IntVal x) (IntVal y) = IntVal $ op x y
-- liftIntOp div _ (IntVal 0) = ExnVal "exn: Division by 0"
liftIntOp _ (ExnVal e) _ = ExnVal e
liftIntOp _ _ (ExnVal e) = ExnVal e
liftIntOp _ _ _ = ExnVal "Cannot lift"

liftBoolOp :: (Bool -> Bool -> Bool) -> Val -> Val -> Val
liftBoolOp op (BoolVal x) (BoolVal y) = BoolVal $ op x y
liftBoolOp _ (ExnVal e) _ = ExnVal e
liftBoolOp _ _ (ExnVal e) = ExnVal e
liftBoolOp _ _ _ = ExnVal "Cannot lift"

liftCompOp :: (Int -> Int -> Bool) -> Val -> Val -> Val
liftCompOp op (IntVal x) (IntVal y) = BoolVal $ op x y
liftCompOp _ (ExnVal e) _ = ExnVal e
liftCompOp _ _ (ExnVal e) = ExnVal e
liftCompOp _ _ _ = ExnVal "Cannot lift"

extractBool :: Val -> Bool
extractBool (BoolVal i) = i
extractBool _ = False

isExn :: Val -> Bool
isExn (ExnVal s) = True
isExn _ = False 

--- Eval
--- ----

eval :: Exp -> Env -> Val

--- ### Constants

eval (IntExp i)  _ = IntVal i
eval (BoolExp i) _ = BoolVal i

--- ### Variables

eval (VarExp s) env = (\x -> if isNothing x then ExnVal "No match in env" else fromJust x) (H.lookup s env)

--- ### Arithmetic

eval (IntOpExp op e1 e2) env = liftIntOp (fromJust (H.lookup op intOps)) (eval e1 env) (eval e2 env)

--- ### Boolean and Comparison Operators

eval (BoolOpExp op e1 e2) env = liftBoolOp (fromJust (H.lookup op boolOps)) (eval e1 env) (eval e2 env)

eval (CompOpExp op e1 e2) env = liftCompOp (fromJust (H.lookup op compOps)) (eval e1 env) (eval e2 env)

--- ### If Expressions

eval (IfExp e1 e2 e3) env = (\x -> if isExn x then x else if extractBool x then eval e2 env else eval e3 env) (eval e1 env)

--- ### Functions and Function Application

eval (FunExp params body) env = CloVal params body env

-- FunExp [String] Exp
-- CloVal [String] Exp Env

eval (AppExp e1 args) env = eval e1 env

-- type Env  = H.HashMap String Val

--- ### Let Expressions

eval (LetExp pairs body) env = undefined

--- Statements
--- ----------

-- Statement Execution
-- -------------------

exec :: Stmt -> PEnv -> Env -> Result
exec (PrintStmt e) penv env = (val, penv, env)
    where val = show $ eval e env

--- ### Set Statements

exec (SetStmt var e) penv env = undefined

--- ### Sequencing

exec (SeqStmt []) penv env = undefined

--- ### If Statements

exec (IfStmt e1 s1 s2) penv env = undefined

--- ### Procedure and Call Statements

exec p@(ProcedureStmt name args body) penv env = undefined

exec (CallStmt name args) penv env = undefined