format elf64 executable 3
include 'call.inc'
include 'io.inc'
include 'inl_itoa.inc'

entry _start

macro TO_STR dest,src { ITOA dest,src,8,16,0,UPPER,0xA }

_start:
    mov rax, 0xA
    TO_STR [buf], rax
    PRINT  [buf], rdx

    TO_STR [buf], [val]
    PRINT  [buf], rdx

    SYSCALL EXIT, 0

segment writeable
val db 255
buf rb 16
