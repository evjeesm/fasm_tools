include 'comptime/build.inc'
include 'comptime/fmt.inc'

TARGET ELF64, OBJECT
include 'os/inl_io.inc'
include 'os/args.inc' ;; TODO:: for now using soft link to x86_64

CODE_SEGMENT
MAIN _start:
    LOAD_ARGC rdi
    PRINT_VALUE rdi, "Arg count: "

    LOAD_ARGV r9
    LOAD_NTH_ARG r12, r9, 2
    ;; deref a pointer to the arg on the stack
    mov r12, [r12]

    ;; replace arg1 with arg2
    SET_NTH_ARG r9, 1, r12

    LOAD_ENVP r9
    SET_NTH_ENV r9, 0, env0_override
    ;; terminate env list
    SET_NTH_ENV r9, 1, 0

    ENVP_COUNT r9, rdi
    PRINT_VALUE rdi

    LOAD_ENVP r9
    PRINT_ARGS r9

    LOAD_ARGV r9
    PRINT_ENV r9

    SYSCALL EXIT, 0


DATA_SEGMENT
arg1_override db "Hello, Arg!", 0
env0_override db "HELLO=Hello, Env!", 0
