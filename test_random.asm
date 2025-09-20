format elf64 executable 3
entry _start

include 'call.inc'
include 'io.inc'

_start:
    SYSCALL GETRANDOM, [random], 16
    PRINT [random], 16
    SYSCALL EXIT, 0

segment readable writable
random rb 16

