module Infer where

import Common

import Control.Monad.Writer (listen)
import Control.Monad.Except (throwError)
import Data.Map.Strict as H (Map, insert, lookup, empty, fromList, singleton)

  {- question 1: fresh instance function -}

freshInst :: PolyTy -> Infer MonoTy
freshInst (Forall qVars tau) = do
  freshVars <- mapM (const freshTau) qVars
  let substitution = H.fromList (zip qVars freshVars)
  return $ apply substitution tau


  {- question 2: occurs check -}

-- | Checks if a given type variable occurs within a monomorphic type.
occurs :: VarId -> MonoTy -> Bool
occurs i tau = i `elem` freeVars tau

  {- question 3: unification -}

unify :: [Constraint] -> Infer Substitution
-- If 0 is empty, return the identity substitution.
unify [] = return substEmpty
unify ((s :~: t) : rest)
    -- (a) Delete rule: If s and t are equal, discard the pair and unify 0'.
  | s == t = unify rest
    -- (c) Decompose rule: Match type constructors.
  | TyConst c1 sArgs <- s, TyConst c2 tArgs <- t, c1 == c2 = do
      let newConstraints = zipWith (:~:) sArgs tArgs
      unify (newConstraints ++ rest)          -- Add decomposed constraints to 0' and unify.
    -- (d) Eliminate rule: If s is a variable and does not occur in t.
  | TyVar v <- s, not (occurs v t) = do
      let subst = substInit v t
      let rest' = map (apply subst) rest  -- Substitute s with t in 0'.
      sigma <- unify rest'                -- Recursively unify the updated constraints.
      return $ substCompose sigma subst   -- Combine the substitution with the result.
    -- Why must Rule (b) be placed after Rule (d) if it has alphabetical precedence SMH
    -- (b) Orient rule: Swap if t is a variable and s is not.
  | TyVar v <- t = unify ((t :~: s) : rest)
  | TyConst c1 _ <- s, TyConst c2 _ <- t, c1 /= c2 = throwError $ Can'tMatch s t -- Type constructors don't match.
  | TyVar v <- s, occurs v t = throwError $ InfiniteType v t -- Occurs check fails.
  | otherwise = throwError $ Can'tMatch s t   -- Catch-all for unexpected cases.  

  {- question 4: type inference -}

infer :: TypeEnv -> Exp -> Infer MonoTy
-- infer env exp = undefined
infer env (ConstExp c) = do                       -- Constants
  freshInst (constTySig c)
infer env (VarExp x) = do
  case H.lookup x env of
    Nothing -> throwError $ LookupError x         -- Throw an error if the variable is not found
    Just sigma -> freshInst sigma
infer env (LetExp x e1 e) = do                    -- Let expression
  (t1, c1) <- listen $ infer env e1
  substitution <- unify c1
  let t1' = apply substitution t1
  let sigma = gen (apply substitution env) t1'
  let env' = H.insert x sigma (apply substitution env)
  (t, c2) <- listen $ infer env' e
  unify (c1 ++ c2)
  return t
infer env (BinOpExp op e1 e2) = do                -- Binary operators
  (t1, c1) <- listen $ infer env e1
  (t2, c2) <- listen $ infer env e2
  tauOp <- freshInst (binopTySig op)
  t <- freshTau
  constrain (funTy t1 (funTy t2 t)) tauOp
  unify (c1 ++ c2)
  return t
infer env (MonOpExp op e1) = do                   -- Unary operators
  (t1, constraints) <- listen $ infer env e1
  tauOp <- freshInst (monopTySig op)
  t <- freshTau
  constrain (funTy t1 t) tauOp
  unify constraints
  return t
infer env (IfExp e1 e2 e3) = do                   -- If-Then-Else
  (t1, c1) <- listen $ infer env e1
  (t2, c2) <- listen $ infer env e2
  (t3, c3) <- listen $ infer env e3
  constrain t1 boolTy
  constrain t2 t3
  unify (c1 ++ c2 ++ c3)
  return t3

inferInit :: TypeEnv -> Exp -> Infer PolyTy
inferInit env e = do
  (tau, constraints) <- listen $ infer env e
  substitution <- unify constraints
  return $ quantifyMonoTy $ apply substitution tau

inferDec :: TypeEnv -> Dec -> Infer (TypeEnv, PolyTy)
inferDec env (AnonDec e') = do
  tau <- inferInit env e'
  return (env, tau)
inferDec env (LetDec x e') = do
  tau <- inferInit env (LetExp x e' (VarExp x))
  return (H.insert x tau env, tau)
inferDec env (LetRec f x e') = do
  tau <- inferInit env (LetRecExp f x e' (VarExp f))
  return (H.insert f tau env, tau)