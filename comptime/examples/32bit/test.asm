include 'comptime/comptime.inc'
include 'arch/x32/syscall.inc'
include 'arch/x32/call.inc'

FORMAT_ELF32_EXECUTABLE
CODE_SEGMENT

MAIN _start:
    CALL foo
    SYSCALL EXIT, 0

foo:
  SYSCALL WRITE, STDOUT, msg, msg.len
  ret

RODATA_SEGMENT
  msg db "hello 32bit!",0xA
  msg.len = $ - msg
