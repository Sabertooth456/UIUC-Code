from z3 import *

from symbolicInterpreter2 import *


def test_case1():
    x = Int('x')
    s1 = Assignment(Var('y'), Const(0))
    cond1 = BinaryComparisonOp(Var('x'), Const(0), '==')
    cond2 = BinaryComparisonOp(Var('x'), Const(5), '==')
    cond3 = BinaryComparisonOp(Var('x'), Const(10), '<')
    s2 = Assignment(Var('y'), Const(1))
    s3 = Assignment(Var('y'), Const(2))
    s4 = Assignment(Var('y'), Const(3))
    s5 = Assignment(Var('z'), BinaryArithOp(Var('x'), Const(2), '+'))
    s6 = Assignment(Var('z'), BinaryArithOp(Var('x'), Const(3), '-'))
    s7 = Assignment(Var('y'), BinaryArithOp(Var('y'), Const(5), '+'))
    s8 = Assert(BinaryComparisonOp(Var('y'), Const(7), '=='))

    program = Sequence(
        s1,
        Sequence(Ite(cond1,
            s2,
            Ite(cond2, s3, Ite(cond3, s4, s5))
        ),
        Sequence(s6,
        Sequence(s7, s8)))
    )

    pathCondition = x==5
    store = {'x': x}
    executeCommand(pathCondition, store, program)


def test_case2():
    x = Int('x')
    s1 = Assignment(Var('y'), Const(0))
    cond1 = BinaryComparisonOp(Var('x'), Const(0), '==')
    cond2 = BinaryComparisonOp(Var('x'), Const(5), '==')
    cond3 = BinaryComparisonOp(Var('x'), Const(10), '<')
    s2 = Assignment(Var('y'), Const(1))
    s3 = Assignment(Var('y'), Const(2))
    s4 = Assignment(Var('y'), Const(3))
    s5 = Assignment(Var('z'), BinaryArithOp(Var('x'), Const(2), '+'))
    s6 = Assignment(Var('z'), BinaryArithOp(Var('x'), Const(3), '-'))
    s7 = Assignment(Var('y'), BinaryArithOp(Var('y'), Const(5), '+'))
    s8 = Assert(BinaryComparisonOp(Var('y'), Const(7), '!='))

    program = Sequence(
        s1,
        Sequence(Ite(cond1,
            s2,
            Ite(cond2, s3, Ite(cond3, s4, s5))
        ),
        Sequence(s6,
        Sequence(s7, s8)))
    )

    pathCondition = x==5
    store = {'x': x}
    executeCommand(pathCondition, store, program)


if __name__ == '__main__':
    # Test 1 should print "ASSERT PASSED"
    test_case1()
    # Test 2 should print "ASSERT FAILED"
    test_case2()

    # FEEL FREE TO ADD MORE TESTS HERE