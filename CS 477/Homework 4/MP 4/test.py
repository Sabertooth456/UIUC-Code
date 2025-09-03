from expression import *
from command import *
from ytlin4_interval import executeCommand, set_max_iterations

"""
    Consider a different initial interval for z:
    x = [3, 3], y = [3, 5], z = [10, 15]
    while x > 0:
        z = x * y
        x = x - 1
        
    SOLUTION (widenning after 1st itr):
        z = [-inf, 15]
        x = [-inf, 0]

    Expected output:
    ASSERT PASSED
    ASSERT PASSED
    ASSERT PASSED
    ASSERT PASSED
"""
def test_case1(executeCommand):
    store = {'x': [3, 3], 'y': [3, 5], 'z': [10, 15]}
    cond = BinaryComparisonOp(Var('x'), Const(0), '>')
    s1 = Assignment(Var('z'), BinaryArithOp(Var('x'), Var('y'), '*'))
    s2 = Assignment(Var('x'), BinaryArithOp(Var('x'), Const(1), '-'))
    statements = Sequence(s1, s2)
    while_statement = While(cond, statements)

    # Start Widening right after 1st itr
    set_max_iterations(1)
    # ASSERT PASSED
    assert_statement = Assert(BinaryComparisonOp(Var('z'), Const(-float('inf')), '>='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    # ASSERT PASSED
    assert_statement = Assert(BinaryComparisonOp(Var('z'), Const(15), '<='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    # ASSERT PASSED
    assert_statement = Assert(BinaryComparisonOp(Var('x'), Const(-float('inf')), '>='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    # ASSERT PASSED
    assert_statement = Assert(BinaryComparisonOp(Var('x'), Const(0), '<='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)


"""
    x = [0, 0], y = [20, 20], z = [15, 15]
    while x <= 10:
        if z <= 10:
            x = x + 1
        else:  
            y = y - 1
            x = x + y
        z = z - 2
    
    (max_iterations = 1)
    SOL:
    {'x': [10, inf], 'y': [-inf, 20], 'z': [-inf, 15]}
ASSERT PASSED
ASSERT FAILED
ASSERT PASSED
ASSERT FAILED
ASSERT PASSED
ASSERT FAILED

"""
def test_case2(executeCommand):
    store = {'x': [0, 0], 'y': [20, 20], 'z': [15, 15]}
    cond = BinaryComparisonOp(Var('x'), Const(10), '<=')
    cond_if = BinaryComparisonOp(Var('z'), Const(10), '<=')
    s_if = Assignment(Var('x'), BinaryArithOp(Var('x'), Const(1), '+'))
    s_else_1 = Assignment(Var('y'), BinaryArithOp(Var('y'), Const(1), '-'))
    s_else_2 = Assignment(Var('x'), BinaryArithOp(Var('x'), Var('y'), '+'))
    s1 = Ite(cond_if, s_if, Sequence(s_else_1, s_else_2))
    s2 = Assignment(Var('z'), BinaryArithOp(Var('z'), Const(2), '-'))
    statements = Sequence(s1, s2)
    while_statement = While(cond, statements)

    set_max_iterations(1)
    assert_statement = Assert(BinaryComparisonOp(Var('x'), Const(10), '>='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    assert_statement = Assert(BinaryComparisonOp(Var('x'), Const(11), '>='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    assert_statement = Assert(BinaryComparisonOp(Var('y'), Const(20), '<='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    assert_statement = Assert(BinaryComparisonOp(Var('y'), Const(19), '<='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    assert_statement = Assert(BinaryComparisonOp(Var('z'), Const(15), '<='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    assert_statement = Assert(BinaryComparisonOp(Var('z'), Const(14), '<='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)



if __name__ == '__main__':
    print('---------Test1---------')
    test_case1(executeCommand)
    print('---------Test2---------')
    test_case2(executeCommand)
    # ADD MORE