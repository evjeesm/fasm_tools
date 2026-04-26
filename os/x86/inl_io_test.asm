include 'build.inc'
TARGET ELF32, EXECUTABLE

include 'inl_io.inc'

CODE_SEGMENT
MAIN _start:
  PRINT_INL "Hello, World",0xA

  PRINT_CSTR [my_msg]

  PRINT_VALUE [value], "Value: "


  SYSCALL EXIT, 0



RODATA_SEGMENT
my_msg db "NULL-terminated",0xA,0
value dd 10



