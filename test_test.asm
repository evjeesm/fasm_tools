format elf64 executable 3

include 'inl_itoa.inc'
include 'call.inc'
include 'io.inc'
include 'test.inc'

segment executable readable
entry _start


_start:
    mov rax, 70
    mov rdx, 70

    ASSERT_EQ rax, rdx, "Compare registers"
    ASSERT_EQ 70, 70, "Compare numerics"
    ASSERT_EQ rax, 70, "Register with Numeric"
    CONSTANT_1 = 2001
    CONSTANT_2 = 2000
    ASSERT_EQ CONSTANT_1, CONSTANT_2, "CONSTANTS"
    ASSERT_NE CONSTANT_1, CONSTANT_2, "Not eq"

    SYSCALL EXIT, 0


segment readable writable

