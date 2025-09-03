class Expression:
    def __init__(self):
        pass 

class Const(Expression):
    def __init__(self, c):
        self.const = c

class Var(Expression):
    def __init__(self, name):
        self.name = name 

class UnaryOp(Expression):
    def __init__(self, lhs, op):
        self.lhs = lhs
        self.op = op
        assert op in ['not']

class BinaryArithOp(Expression):
    def __init__(self, lhs, rhs, op):
        self.lhs = lhs
        self.rhs = rhs
        self.op = op
        assert op in ['+', '-', '*']

class BinaryComparisonOp(Expression):
    def __init__(self, lhs, rhs, op):
        self.lhs = lhs
        self.rhs = rhs
        self.op = op
        assert op in ['<', '<=', '>', '>=', '==']

class BinaryLogicalOp(Expression):
    def __init__(self, lhs, rhs, op):
        self.lhs = lhs
        self.rhs = rhs
        self.op = op
        assert op in ['and', 'or', 'implies']
