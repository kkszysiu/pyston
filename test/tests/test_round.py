 
assert round(0) == 0.0
assert round(0.2) == 0.0
assert round(0.5) == 1.0
assert round(-0.165) == -0.0
assert round(-1) == -1.0

try:
    round(-1, 0.6)
    assert 'should throw TypeError: \'float\' object cannot be interpreted as an index' == False
except TypeError as e:
    assert e.message == '\'float\' object cannot be interpreted as an index'

try:
    round('test')
    assert 'should throw TypeError: a float is required' == False
except TypeError as e:
    assert e.message == 'a float is required'

assert 1.0 == round(1.0)
assert 1.0 == round(1.1)
assert 2.0 == round(1.9)
assert 2.0 == round(1.5)
assert -2.0 == round(-1.5)
assert -2.0 == round(-1.5)
assert -2.0 == round(-1.5, 0)
assert -2.0 == round(-1.5, 0)

assert 22.2 == round(22.222222, 1)
#assert 20.0 == round(22.22222, -1)
#assert 0.0 == round(22.22222, -2)

##assert round(123.456, -308) == 0.0
##assert round(123.456, -700) == 0.0
##assert round(123.456, -2**100) == 0.0

assert round(0, 2) == 0.0
assert round(1, 8) == 1.0
assert round(2.675, 2) == 2.67
assert round(-2.567, 2) == -2.57
assert round(-2.675, 2) == -2.67
assert round(2.67556756, 6) == 2.675568

assert round(2.6755675699999999999999999999999999999999999999999999999999999999999999999999999999999999999999, 40) == 2.67556757