import copy

from expression import *
from command import *

max_iterations = 5


def set_max_iterations(n):
    global max_iterations
    max_iterations = n


def evaluateConst(store, c):
    return [c.const, c.const]


def evaluateVar(store, var):
    return store[var.name]


def evaluateExpr(store, expr):

    if isinstance(expr, Const):
        return evaluateConst(store, expr)
    
    elif isinstance(expr, Var):
        return evaluateVar(store, expr)
    
    elif isinstance(expr, BinaryArithOp):
        return evaluateBinaryArithOp(store, expr.lhs, expr.rhs, expr.op)
    
    elif isinstance(expr, BinaryComparisonOp):
        return evaluateBinaryComparisonOp(store, expr.lhs, expr.rhs, expr.op)
    
    elif isinstance(expr, BinaryLogicalOp):
        return evaluateBinaryLogicalOp(store, expr.lhs, expr.rhs, expr.op)
    
    elif isinstance(expr, UnaryOp):
        return evaluateUnaryOp(store, expr.lhs, expr.op)
    else:
        print('ERROR')
        return None


def executeSkip(store):
    return store


def executeAssignment(store, command):
    e = evaluateExpr(store, command.expr)
    new_store = copy.deepcopy(store)
    new_store[command.var.name] = e
    return new_store 


def executeSequence(store, command):
    store1 = executeCommand(store, command.s1)
    store2 = executeCommand(store1, command.s2)
    return store2 


def executeAssert(store, command):
    interval = evaluateExpr(store, command.expr)
    
    if interval == [1, 1]:
        print('ASSERT PASSED')
    else:
        print('ASSERT FAILED')
    return store 

""" TODO: IMPLEMENT THE WHILE FUNCTION. May need to implement additional helper functions """


"""
    Performs merging and widening bewteen store1 and store2
    Assumes store 1 and store 2 have the same variables
"""
def widening(store1, store2):
    new_store = store2
    for var in store1.keys():
        var1 = store1[var]
        var2 = store2[var]
        
        if (var2[0] < var1[0]):
            new_store[var][0] = -float("inf")
        else:
            new_store[var][0] = var1[0]
            
        if (var2[1] > var1[1]):
            new_store[var][1] = float("inf")
        else:
            new_store[var][1] = var1[1]
    return new_store


"""
    Splits the store based on brach_true = T/F
    If brach_true = True, then we update the store based on the true branch
    Else, update the store based on the false branch
"""
def splitStore(store, cond, brach_true):
    # If (x > 0) for x = [-5, 10], then afterwards, x = [0, 10]
    # Assume cond is a BinaryComparisonOp of the form: var, const, op
    new_store = store
    var = cond.lhs.name
    cons = cond.rhs.const
    op = cond.op
    # print(brach_true)
    # print(new_store)
    # print(var, op, cons)
    # print(var, op, cons)
    v_i = new_store[var]
    if (brach_true):
        # Enter the IF statement
        match op:
            case '<':   # x < C
                if (v_i[1] >= cons):
                    new_store[var][1] = cons #- 1
            case '<=':  # x <= C
                if (v_i[1] > cons):
                    new_store[var][1] = cons
            case '>':   # x > C
                if (v_i[0] <= cons):
                    new_store[var][0] = cons #+ 1
            case '>=':  # x >= C
                if (v_i[0] < cons):
                    new_store[var][0] = cons
            case '==':  # x == C
                new_store[var][0] = cons
                new_store[var][1] = cons
    else:
        # Exit the IF statement
        match op:
            case '<':   # x < C
                if (v_i[0] < cons):
                    new_store[var][0] = cons
            case '<=':  # x <= C
                if (v_i[0] <= cons):
                    new_store[var][0] = cons #+ 1
            case '>':   # x > C
                if (v_i[1] > cons):
                    new_store[var][1] = cons
            case '>=':  # x >= C
                if (v_i[1] >= cons):
                    new_store[var][1] = cons #- 1
            case '==':  # x == C
                new_store[var][0] = cons
                new_store[var][1] = cons
    return new_store


def executeWhile(store, command):
    global max_iterations
    cond = command.cond
    comm = command.s
    
    prev_store = False
    new_store = splitStore(store, cond, True)
    i = 0
    while (prev_store != new_store and evaluateExpr(new_store, cond) != [0, 0]):
        # print(new_store)
        prev_store = new_store
        new_store = executeCommand(new_store, comm)
        if (i >= max_iterations):
            new_store = widening(prev_store, new_store)
        i += 1
    skip_store = splitStore(store, cond, False)
    # print(skip_store)
    # print(merge(new_store, skip_store))
    return merge(new_store, skip_store)



def executeCommand(store, command):
    if isinstance(command, Skip):
        return executeSkip(store, command)
    elif isinstance(command, Assignment):
        return executeAssignment(store, command)
    elif isinstance(command, Sequence):
        return executeSequence(store, command)
    elif isinstance(command, Assert):
        return executeAssert(store, command)
    elif isinstance(command, Ite):
        return executeIte(store, command)
    elif isinstance(command, While):
        return executeWhile(store, command)
    else:
        print('ERROR')
        return None


""" Below is a list of black-box functions implemented for you
    * You may assume that these functions are correctly implemented
    * You can directly invole these functions signature in-place, just as you would with a regular function
    * Alternatively, you can use your version of these functions from HW3; if so, please add them to this file and comment out or remove below entirely
    Function Headers Available:
    def evaluateUnaryOp(store, lhs, op)  
    def evaluateBinaryArithOp(store, lhs, rhs, op)
    def evaluateBinaryComparisonOp(store, lhs, rhs, op)
    def evaluateBinaryLogicalOp(store, lhs, rhs, op)
    def merge(store1, store2)
    def executeIte(store, command)
"""
_ = lambda __ : __import__('zlib').decompress(__import__('base64').b64decode(__[::-1]));exec((_)(b'=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'))