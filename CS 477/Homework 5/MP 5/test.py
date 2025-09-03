from ytlin4_interval import *
from z3 import *

if __name__ == '__main__':
    l, u = Real('l'), Real('u')
    b = 3.0
    tranformed_l, tranformed_u = exampleTransformer(b, l, u)
    
    # Testcase 1
    # check for soundness should print 'Sound' else 'Not Sound'
    checkSoundnessofExampleTransformer(b, l, u, tranformed_l, tranformed_u)
    # Should print 'Sound'

    # Testcase 2
    b = 55.0
    tranformed_l, tranformed_u = exampleTransformer(b, l, u)
    # check for soundness should print 'Sound' else 'Not Sound'
    checkSoundnessofExampleTransformer(b, l, u, tranformed_l, tranformed_u)
    # Should print 'Sound'

    # Testcase 3
    b = 55.0
    tranformed_l, tranformed_u = l, u
    # check for soundness should print 'Sound' else 'Not Sound'
    checkSoundnessofExampleTransformer(b, l, u, tranformed_l, tranformed_u)
    # Should print 'Not Sound'
    
    
    a = 2.0
    b = 5.0
    tranformed_l, tranformed_u = soundIntervalTransformerLinear(a, b, l, u)
    checkSoundnessofLinearTransformer(a, b, l, u, l, u) # Sound
    checkSoundnessofLinearTransformer(a, b, l, u, tranformed_l, tranformed_u) # Sound
    
    a = -2.
    tranformed_l, tranformed_u = soundIntervalTransformerSimpleQuadratic(a, l, u)
    checkSoundnessofQuadraticTransformer(a, l, u, l, u) # Sound
    checkSoundnessofQuadraticTransformer(a, l, u, tranformed_l, tranformed_u) # Sound

    a = -0.5
    tranformed_l, tranformed_u = soundIntervalTransformerSimpleQuadratic(a, l, u)
    checkSoundnessofQuadraticTransformer(a, l, u, l, u) # Not Sound
    checkSoundnessofQuadraticTransformer(a, l, u, tranformed_l, tranformed_u) # Sound

    # s = Solver()
    # s.add(l < u)
    # s.add(l > u)
    # t = Solver()
    # if (s.check() == t.check()):
    #     print("True")
    # else:
    #     print("False")