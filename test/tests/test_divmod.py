assert divmod(0, 2) == (0, 0)
assert divmod(1, 2) == (0, 1)
assert divmod(2, 1) == (2, 0)
assert divmod(2, 1000) == (0, 2)
assert divmod(20000, 1000) == (20, 0)
assert divmod(20000, 999) == (20, 20)
assert divmod(-20000, 999) == (-21, 979)
assert divmod(-20000, -999) == (20, -20)
assert divmod(-2, -9) == (0, -2)
print 'ok'
print 'divmod(2, 9.0)', divmod(2, 9.0)
assert divmod(2, 9.0) == (0.0, 2.0)
assert divmod(2.0, 1) == (2.0, 0.0)
assert divmod(2.0, 1.6) == (1.0, 0.3999999999999999)
assert divmod(2.5, 2) == (1.0, 0.5)
assert divmod(2.5, 2.0) == (1.0, 0.5)
assert divmod(-2, -9.0) == (0.0, -2.0)

try:
    divmod(1, 0)
    assert 'Should throw ZeroDivisionError: integer division or modulo by zero' == False
except ZeroDivisionError as e:
    assert e.message == 'integer division or modulo by zero'

