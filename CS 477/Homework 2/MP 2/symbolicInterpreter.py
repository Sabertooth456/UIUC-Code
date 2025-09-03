from z3 import *
import copy

from expression import *
from command import *

"""PLEASE ONLY MODIFY THE TODO SECTIONS"""

def evaluateConst(pathCondition, store, c):
    return c.const

def evaluateVar(pathCondition, store, var):
    return store[var.name]

# ['not']
def evaluateUnaryOp(pathCondition, store, lhs, op):
    # TODO
    return Not(evaluateExpr(pathCondition, store, lhs))

# ['+', '-', '*']
def evaluateBinaryArithOp(pathCondition, store, lhs, rhs, op):
    # TODO
    match (op):
        case '+':
            return evaluateExpr(pathCondition, store, lhs) + evaluateExpr(pathCondition, store, rhs)
        case '-':
            return evaluateExpr(pathCondition, store, lhs) - evaluateExpr(pathCondition, store, rhs)
        case '*':
            return evaluateExpr(pathCondition, store, lhs) * evaluateExpr(pathCondition, store, rhs)

# ['<', '<=', '>', '>=', '==', '!=']
def evaluateBinaryComparisonOp(pathCondition, store, lhs, rhs, op):
    # TODO
    match (op):
        case '<':
            return evaluateExpr(pathCondition, store, lhs) < evaluateExpr(pathCondition, store, rhs)
        case '<=':
            return evaluateExpr(pathCondition, store, lhs) <= evaluateExpr(pathCondition, store, rhs)
        case '>':
            return evaluateExpr(pathCondition, store, lhs) > evaluateExpr(pathCondition, store, rhs)
        case '>=':
            return evaluateExpr(pathCondition, store, lhs) >= evaluateExpr(pathCondition, store, rhs)
        case '==':
            return evaluateExpr(pathCondition, store, lhs) == evaluateExpr(pathCondition, store, rhs)
        case '!=':
            return evaluateExpr(pathCondition, store, lhs) != evaluateExpr(pathCondition, store, rhs)

# ['and', 'or', 'implies']
def evaluateBinaryLogicalOp(pathCondition, store, lhs, rhs, op):
    # TODO
    match (op):
        case 'and':
            return And(evaluateExpr(pathCondition, store, lhs), evaluateExpr(pathCondition, store, rhs))
        case 'or':
            return Or(evaluateExpr(pathCondition, store, lhs), evaluateExpr(pathCondition, store, rhs))
        case 'implies':
            return Implies(evaluateExpr(pathCondition, store, lhs), evaluateExpr(pathCondition, store, rhs))


def evaluateExpr(pathCondition, store, expr):

    if isinstance(expr, Const):
        return evaluateConst(pathCondition, store, expr)
    
    elif isinstance(expr, Var):
        return evaluateVar(pathCondition, store, expr)
    
    elif isinstance(expr, BinaryArithOp):
        return evaluateBinaryArithOp(pathCondition, store, expr.lhs, expr.rhs, expr.op)
    
    elif isinstance(expr, BinaryComparisonOp):
        return evaluateBinaryComparisonOp(pathCondition, store, expr.lhs, expr.rhs, expr.op)
    
    elif isinstance(expr, BinaryLogicalOp):
        return evaluateBinaryLogicalOp(pathCondition, store, expr.lhs, expr.rhs, expr.op)
    
    elif isinstance(expr, UnaryOp):
        return evaluateUnaryOp(pathCondition, store, expr.lhs, expr.op)
    else:
        print('ERROR')
        return None



def executeSkip(pathCondition, store):
    return pathCondition, store


def executeAssignment(pathCondition, store, command):
    # TODO
    store[command.var] = evaluateExpr(pathCondition, store, command.expr)
    return pathCondition, store

def executeSequence(pathCondition, store, command):
    # TODO
    store[command.s1.var.name] = evaluateExpr(pathCondition, store, command.s1.expr)
    return executeCommand(pathCondition, store, command.s2)

def executeAssert(pathCondition, store, command):
    e = evaluateExpr(pathCondition, store, command.expr)
    s = Solver()
    s.add(Not(Implies(pathCondition, e)))
    c = s.check()
    if c == unsat:
        print('ASSERT PASSED')
    else:
        print('ASSERT FAILED')
    return pathCondition, store 


def executeCommand(pathCondition, store, command):
    if isinstance(command, Skip):
        return executeSkip(pathCondition, store, command)
    elif isinstance(command, Assignment):
        return executeAssignment(pathCondition, store, command)
    elif isinstance(command, Sequence):
        return executeSequence(pathCondition, store, command)
    elif isinstance(command, Assert):
        return executeAssert(pathCondition, store, command)
    elif isinstance(command, Ite):
        print('NOT IMPLEMENTED')
        return None
    else:
        print('ERROR')
        return None
