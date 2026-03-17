include 'comptime.inc'
include 'call.inc'
include 'syscall.inc'
include 'inl_io.inc'
include 'inl_itoa.inc'
include 'args.inc'

FORMAT_ELF64_EXECUTABLE

if ~ PAYLOAD_CALL eq & PAYLOAD_CALL = 1
  SKIP=8
else
  SKIP=0
end if

CODE_SEGMENT
MAIN _start:

    LOAD_ARGC rdi, SKIP
    PRINT_VALUE rdi, <"Payload is Running!!!",10,"Arg count: ">

    LOAD_ENVP rbx, SKIP
    PRINT_ENV rbx

    LOAD_ARGV rbx, SKIP
    PRINT_ARGS rbx

if ~ PAYLOAD_CALL eq & PAYLOAD_CALL = 1
    ret
else
    SYSCALL EXIT, 0
end if

DATA_SEGMENT
arg1_override db "Hello, Arg!", 0
env0_override db "HELLO=Hello, Env!", 0
