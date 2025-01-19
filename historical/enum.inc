include 'comptime.inc'

macro ENUM [values*]
 {
 common
    local count,m
    count = 0
 forward
    m = 0
    match a == b, values
    \{
        m = 1
        a = b
        count = b + 1
    \}
    if ~ m
        values = count
        count = count + 1
    end if
 }

ENUM \
    _A = 10,   \
    _B = _A+2, \
    _C,        \
    _D         \

FMT '_A:' %D _A ' _B: ' _B ' _C: ' _C ' _D: ' _D %NL
