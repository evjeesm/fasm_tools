format elf64 executable 3

include 'inl_itoa.inc'
include 'call.inc'
include 'io.inc'
include 'assert.inc'

segment executable readable
entry _start


_start:
    mov rax, 70
    mov rdx, 70

    ASSERT_EQ rax, rdx, "Compare registers"
    ASSERT_EQ 70, 70, "Compare numerics"
    ASSERT_EQ rax, 70, "Register with Numeric"
    CONSTANT_1 = 2000
    CONSTANT_2 = 2000
    ASSERT_EQ CONSTANT_1, CONSTANT_2, "CONSTANTS"
    ;; ASSERT_NE CONSTANT_1, CONSTANT_2, "Not eq"

    mov rax, 0xAAAAAAAA_AAAAAAAA
    mov rdx, 0xFFFFFFFF
    mov rax, rdx
    ASSERT_EQ rax, 0xFFFFFFFF, "Compare registers"

    ASSERT_POSITIVE 1, "Should be positive"
    ASSERT_ZERO 0, "Should be zero"
    ASSERT_POSITIVE +1, "Should be positive"
    _ASSERT_RANGE _OP_EQ, src, dest, src_len, "Hoho"
    ;; CCALL assert_strcpy, [other], [src], 5, src_len
    ;; CCALL assert_str_begin
    ;; CCALL assert_str_build, [src], src_len
    ;; PRINT [assert_str], [assert_str_index]
    SYSCALL EXIT, 0


segment readable writable
src db "Hello, Borld", 10
src_len = $ - src

dest db "Hello, World", 10
dest_len = $ - dest

other rb 100
