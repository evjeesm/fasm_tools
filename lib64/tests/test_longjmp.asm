include 'comptime.inc'
FORMAT_ELF64_OBJECT

USE 'setjmp.inc'

CODE_SEGMENT

MAIN _start:
    CCALL setjmp
    ret


