from z3 import *



def exampleTransformer(b: float, l: Real, u: Real):
    # This is an example function that implements the sound interval transformer for f(x) = x + b.
    # return the min and max of the output interval
    return (l+b), (u+b)

def soundIntervalTransformerLinear(a: float, b: float, l: Real, u: Real):
    # This function implements the sound interval transformer for linear functions
    # It takes a linear function f(x) = ax + b and an interval [l, u] and returns the transformed interval
    # f^#([l, u])
    # TODO implement this function see the output of the exampleTransformer function
    # You should try on your own examples no test cases are provided at the moment
    if (a <= 0.):
        return (u * a + b), (l * a + b)
    else:
        return (l * a + b), (u * a + b)


def soundIntervalTransformerSimpleQuadratic(a: float, l: Real, u: Real):
    # This function implements the sound interval transformer for quadratic functions
    # It takes a linear function f(x) = ax^2 and an interval [l, u] and returns the transformed interval
    # f^#([l, u])
    # TODO implement this function see the output of the exampleTransformer function
    # You should try on your own examples no test cases are provided at the moment
    s = Solver()
    s.add((l**2) <= (u**2))
    t = Solver()
    if (s.check() == t.check()):
        if (a <= 0):
            return (a * u**2), (a * l**2)
        else:
            return (a * l**2), (a * u**2)
    else:
        if (a <= 0):
            return (a * l**2), (a * u**2)
        else:
            return (a * u**2), (a * l**2)

def checkSoundnessofExampleTransformer(b: float, l: Real, u: Real, tranformed_l, tranformed_u):
    # This function checks the soundness of the example transformer
    # It takes the original interval [l, u] and the transformed interval [tranformed_l, tranformed_u]
    # and checks if the transformed interval is a sound approximation of the original interval on the function f(x) = x + b 
    # TODO implement this function
    new_l, new_u = exampleTransformer(b, l, u)
    s = Solver()
    s.add(tranformed_l <= new_l)
    s.add(tranformed_u >= new_u)
    t = Solver()
    if (s.check() == t.check()):
        print("Sound")
    else:
        print("Not Sound")

def checkSoundnessofLinearTransformer(a: float, b: float, l: Real, u: Real, tranformed_l, tranformed_u):
    # This function checks the soundness of the example transformer
    # It takes the original interval [l, u] and the transformed interval [tranformed_l, tranformed_u]
    # and checks if the transformed interval is a sound approximation of the original interval on the function f(x) = a * x + b 
    # TODO implement this function
    # You should try on your own examples no test cases are provided at the moment
    new_l, new_u = soundIntervalTransformerLinear(a, b, l, u)
    s = Solver()
    s.add(tranformed_l <= new_l)
    s.add(tranformed_u >= new_u)
    t = Solver()
    if (s.check() == t.check()):
        print("Sound")
    else:
        print("Not Sound")

def checkSoundnessofQuadraticTransformer(a: float, l: Real, u: Real, tranformed_l, tranformed_u):
    # This function checks the soundness of the example transformer
    # It takes the original interval [l, u] and the transformed interval [tranformed_l, tranformed_u]
    # and checks if the transformed interval is a sound approximation of the original interval on the function f(x) = a * x^2 
    # TODO implement this function
    # You should try on your own examples no test cases are provided at the moment
    new_l, new_u = soundIntervalTransformerSimpleQuadratic(a, l, u)
    s = Solver()
    s.add(tranformed_l <= new_l)
    s.add(tranformed_u >= new_u)
    t = Solver()
    if (s.check() == t.check()):
        print("Sound")
    else:
        print("Not Sound")

