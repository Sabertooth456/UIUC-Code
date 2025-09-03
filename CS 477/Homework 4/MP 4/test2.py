from expression import *
from command import *
from ytlin4_interval import executeCommand, set_max_iterations

"""
    x = [3, 3], y = [3, 5], z = [10, 10]
    while x > 0:
        z = x * y
        x = x - 1
    
    SOL (no widening):
        z = [0, 15] 
        x = [-1, 0]
    
    SOL (widenning after 1st itr):
        z = [-inf, inf]
        x = [-inf, 0]

    Expected output:
    ASSERT PASSED
    ASSERT PASSED
    ASSERT PASSED
    ASSERT PASSED
    ASSERT FAILED
    ASSERT FAILED
    ASSERT PASSED
    ASSERT PASSED
    ASSERT FAILED
    ASSERT FAILED
    ASSERT PASSED
    ASSERT PASSED
"""
def test_case1(executeCommand):
    # Start Widening after 10-th itr (converged before any widenning)
    set_max_iterations(10)
    store = {'x': [3, 3], 'y': [3, 5], 'z': [10, 10]}
    cond = BinaryComparisonOp(Var('x'), Const(0), '>')
    s1 = Assignment(Var('z'), BinaryArithOp(Var('x'), Var('y'), '*'))
    s2 = Assignment(Var('x'), BinaryArithOp(Var('x'), Const(1), '-'))
    statements = Sequence(s1, s2)
    while_statement = While(cond, statements)

    # ASSERT PASSED
    assert_statement = Assert(BinaryComparisonOp(Var('z'), Const(0), '>='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    # ASSERT PASSED
    assert_statement = Assert(BinaryComparisonOp(Var('z'), Const(15), '<='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    # ASSERT PASSED
    assert_statement = Assert(BinaryComparisonOp(Var('x'), Const(-1), '>='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    # ASSERT PASSED
    assert_statement = Assert(BinaryComparisonOp(Var('x'), Const(0), '<='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    # ASSERT FAILED
    assert_statement = Assert(BinaryComparisonOp(Var('z'), Const(1), '>='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    # ASSERT FAILED
    assert_statement = Assert(BinaryComparisonOp(Var('z'), Const(14), '<='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    # Start Widening right after 1st itr
    set_max_iterations(1)

    # ASSERT PASSED
    assert_statement = Assert(BinaryComparisonOp(Var('z'), Const(-float('inf')), '>='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    # ASSERT PASSED
    assert_statement = Assert(BinaryComparisonOp(Var('z'), Const(float('inf')), '<='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    # ASSERT FAILED
    assert_statement = Assert(BinaryComparisonOp(Var('z'), Const(0), '>='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    # ASSERT FAILED
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
    This is a case where first few iterations w/o wideining, and then uses
    wideing to help converge
    max_iteration = 4
    x = [3, 3], y = [3, 5], z = [10, 10]
    while x < 5:
        y = y + x
        z = z + 2 * y
        x = x + 1
        
    SOLUTION:
        z = [10, inf]
        y = [3, inf]
        x = [5, 6]

    Expected output:
    ASSERT PASSED
    ASSERT PASSED
    ASSERT PASSED
    ASSERT FAILED
    ASSERT PASSED
    ASSERT PASSED
    ASSERT FAILED
    ASSERT PASSED
"""
def test_case2(executeCommand):
    store = {'x': [3, 3], 'y': [3, 5], 'z': [10, 10]}
    cond = BinaryComparisonOp(Var('x'), Const(5), '<')
    s1 = Assignment(Var('y'), BinaryArithOp(Var('y'), Var('x'), '+'))
    s2 = Assignment(Var('z'), BinaryArithOp(Var('z'), BinaryArithOp(Const(2), Var('y'), '*'), '+'))
    s3 = Assignment(Var('x'), BinaryArithOp(Var('x'), Const(1), '+'))
    statements = Sequence(s1, Sequence(s2, s3))
    while_statement = While(cond, statements)

    # Start Widening right after 4th itr
    set_max_iterations(4)

    # ASSERT PASSED
    assert_statement = Assert(BinaryComparisonOp(Var('x'), Const(5), '>='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    # ASSERT PASSED
    assert_statement = Assert(BinaryComparisonOp(Var('x'), Const(6), '<='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    # ASSERT PASSED
    assert_statement = Assert(BinaryComparisonOp(Var('y'), Const(3), '>='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    # ASSERT FAILED
    assert_statement = Assert(BinaryComparisonOp(Var('y'), Const(4), '>='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    # ASSERT PASSED
    assert_statement = Assert(BinaryComparisonOp(Var('y'), Const(float('inf')), '<='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    # ASSERT PASSED
    assert_statement = Assert(BinaryComparisonOp(Var('z'), Const(10), '>='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    # ASSERT FAILED
    assert_statement = Assert(BinaryComparisonOp(Var('z'), Const(11), '>='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

    # ASSERT PASSED
    assert_statement = Assert(BinaryComparisonOp(Var('z'), Const(float('inf')), '<='))
    program = Sequence(while_statement, assert_statement)
    executeCommand(store, program)

if __name__ == '__main__':
    print('---------Test1---------')
    test_case1(executeCommand)
    print('---------Test2---------')
    test_case2(executeCommand)
    # ADD MORE