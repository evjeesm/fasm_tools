format elf64

include 'io.inc'
include 'call.inc'

section '.text' executable
    public _start
    extrn write_all
    extrn write

_start:
    SYSCALL WRITE, 1, [buf], 8
    CCALL write, 1, [buf], 8
    CCALL write_all, 1, [buf], 8
    SYSCALL EXIT, 0

section '.data' writable
buf db 'abcdefghijklmn'
