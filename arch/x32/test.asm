include 'call.inc'
include 'syscall.inc'
include 'syscall_nr.inc'

format elf executable 3

segment readable executable
entry _start


_start:
  CALL foo, 12
  SYSCALL EXIT, eax

foo:
  add edi, edi
  mov eax, edi
  ret
