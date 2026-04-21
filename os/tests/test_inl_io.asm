include 'build.inc'

;; fasm should peek up architecture specific includes after target specified
TARGET ELF64, OBJECT

;; this platform independed file will do the switch
include 'os/inl_io.inc'

CODE_SEGMENT
MAIN _start:
    PRINT [msg], msg.size
    SYSCALL EXIT, 0

DATA_SEGMENT
    msg BUFFER "Hello, Inl IO!",10
