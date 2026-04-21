include 'comptime/fmt.inc'
if definite ARCH
  DEBUG
    FMT 'ARCH: ' %D ARCH %NL
    FMT 'BUILD_TYPE: ' %D BUILD_TYPE %NL
  END_DEBUG
  if ARCH = ARCH_X32
    include 'x32/call.inc'
    include 'x32/syscall.inc'
    include 'x32/syscall_nr.inc'
    DEBUG
      display "INCLUDE ARCH X32", 10
    END_DEBUG
  else if ARCH = ARCH_X64
    include 'x64/call.inc'
    include 'x64/syscall.inc'
    include 'x64/syscall_nr.inc'
    DEBUG
      display "INCLUDE ARCH X64", 10
    END_DEBUG
  else
    err ;; Unknown architecture
  end if
else
  err ;; Unspecified architecture
end if
