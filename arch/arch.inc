include 'comptime/fmt.inc'
if definite ARCH
  DEBUG
    FMT 'ARCH: ' %D ARCH %NL
    FMT 'BUILD_TYPE: ' %D BUILD_TYPE %NL
  END_DEBUG
  if ARCH = ARCH_X86
    include 'x86/call.inc'
    include 'x86/syscall.inc'
    include 'x86/syscall_nr.inc'
    include 'x86/pointer.inc'
    DEBUG
      display "INCLUDE ARCH X86", 10
    END_DEBUG
  else if ARCH = ARCH_X86_64
    include 'x86_64/call.inc'
    include 'x86_64/syscall.inc'
    include 'x86_64/syscall_nr.inc'
    include 'x86_64/pointer.inc'
    DEBUG
      display "INCLUDE ARCH X86_64", 10
    END_DEBUG
  else
    err ;; Unknown architecture
  end if
else
  err ;; Unspecified architecture
end if
