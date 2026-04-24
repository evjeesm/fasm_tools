include 'build.inc'

TARGET ELF64, OBJECT
include 'os/inl_io.inc'

CODE_SEGMENT
MAIN _start:

  REL_TO_ABS rdi, msg1p
  PRINT_CSTR [rdi]

  DEREF_REL rdi, **[msg2ppp]
  PRINT_CSTR [rdi]

  SYSCALL EXIT, 0

DATA_SEGMENT

msg1 db "Hello,", 0
msg2 db "World", 0

msg1p REL_PTR msg1
msg2p REL_PTR msg2

msg1pp REL_PTR msg1p
msg1ppp REL_PTR msg1pp

msg2pp REL_PTR msg2p
msg2ppp REL_PTR msg2pp


