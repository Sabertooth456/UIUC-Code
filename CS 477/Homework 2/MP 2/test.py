from z3 import *

from symbolicInterpreter import *


""" (A -> B) OR (B -> A) (ASSERT PASSED) """
def test_case1():
    A = Bool('A')
    B = Bool('B')
    A_to_B = Assignment(Var('z'), BinaryLogicalOp(Var('A'), Var('B'), 'implies'))
    B_to_C = Assignment(Var('w'), BinaryLogicalOp(Var('B'), Var('A'), 'implies'))
    s = Assert(BinaryLogicalOp(Var('z'), Var('w'), 'or'))
    s2 = Sequence(A_to_B, Sequence(B_to_C, s))
    program = s2
    pathCondition = True 
    store = {'A': A, 'B': B}
    executeCommand(pathCondition, store, program)


""" (A -> B) AND (B -> A) (ASSERT FAILED) """
def test_case2():
    A = Bool('A')
    B = Bool('B')
    A_to_B = Assignment(Var('z'), BinaryLogicalOp(Var('A'), Var('B'), 'implies'))
    B_to_C = Assignment(Var('w'), BinaryLogicalOp(Var('B'), Var('A'), 'implies'))
    s = Assert(BinaryLogicalOp(Var('z'), Var('w'), 'and'))
    s2 = Sequence(A_to_B, Sequence(B_to_C, s))
    program = s2
    pathCondition = True 
    store = {'A': A, 'B': B}
    executeCommand(pathCondition, store, program)


if __name__ == '__main__':
    # Test 1 should print "ASSERT PASSED"
    test_case1()
    # Test 2 should print "ASSERT FAILED"
    test_case2()

    # FEEL FREE TO ADD MORE TESTS HERE
