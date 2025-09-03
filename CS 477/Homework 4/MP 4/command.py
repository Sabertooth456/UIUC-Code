class Statement:
    def __init__(self):
        pass

class Skip(Statement):
    def __init__(self):
        pass

class Assignment(Statement):
    def __init__(self, var, expr):
        self.var = var 
        self.expr = expr 

class Sequence(Statement):
    def __init__(self, s1, s2):
        self.s1 = s1
        self.s2 = s2

class Ite(Statement):
    def __init__(self, cond, s1, s2):
        self.cond = cond
        self.s1 = s1
        self.s2 = s2

class While(Statement):
    def __init__(self, cond, s):
        self.cond = cond
        self.s = s

class Assert(Statement):
    def __init__(self, expr):
        self.expr = expr
        pass 

