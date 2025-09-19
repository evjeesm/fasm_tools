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

    mov rax, 0xAAAA_AAAA_AAAA_AAAA
    mov rdx, 0xFFFF_FFFF
    mov rax, rdx
    ASSERT_EQ rax, 0xFFFF_FFFF, "Compare registers"

    ASSERT_POSITIVE 1, "Should be positive"
    ASSERT_ZERO 0, "Should be zero"
    ASSERT_POSITIVE +1, "Should be positive"
    ASSERT_RANGE_ALL_NE src, dest, src_len, "Hoho"
    ASSERT_RANGE_CHAR_ALL_EQ src, 'H', src_len, "Hoho"

    SYSCALL EXIT, 0


segment readable writable
src  db "Hello, World", 10
src_len = $ - src

dest db "Hello, World", 10
dest_len = $ - dest

other rb 100
