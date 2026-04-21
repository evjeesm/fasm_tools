include 'comptime.inc'
include '../x64/inl_io.inc'

FORMAT_ELF64_OBJECT

CODE_SEGMENT
MAIN _start:
    PRINT [msg], msg.size
    SYSCALL EXIT, 0

DATA_SEGMENT
    msg BUFFER "Hello, Inl IO!",10
