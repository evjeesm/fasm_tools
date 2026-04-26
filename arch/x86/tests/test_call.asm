include 'comptime/build.inc'

TARGET ELF32, EXECUTABLE
include 'os/inl_io.inc'

CODE_SEGMENT
MAIN _start:
  CALL foo, 69, 420
  CALL foo_no_prelude, 69, 420
  SYSCALL EXIT, eax

foo:
  push ebp
  mov ebp, esp

  PRINT_VALUE [ebp+4+4], "First Arg:"
  PRINT_VALUE [ebp+4+8], "Second Arg:"

  leave
  ret

foo_no_prelude:
  PRINT_VALUE [esp+4], "First Arg:"
  PRINT_VALUE [esp+8], "Second Arg:"
  ret
