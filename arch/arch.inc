include 'comptime/build.inc'
include 'comptime/fmt.inc'

STATIC_ASSERT definite ARCH, "ARCH should be defined!"
DEBUG
  FMT 'ARCH: ' %D ARCH %NL
  FMT 'BUILD_TYPE: ' %D BUILD_TYPE %NL
END_DEBUG
repeat 1
match =ARCH_X86, ARCH {
  include 'x86/call.inc'
  include 'x86/syscall.inc'
  include 'x86/syscall_nr.inc'
  include 'x86/pointer.inc'
  DEBUG
    display "INCLUDE ARCH X86", 10
  END_DEBUG
  break
}
match =ARCH_X86_64, ARCH {
  include 'x86_64/call.inc'
  include 'x86_64/syscall.inc'
  include 'x86_64/syscall_nr.inc'
  include 'x86_64/pointer.inc'
  DEBUG
    display "INCLUDE ARCH X86_64", 10
  END_DEBUG
  break
}
CRITICAL "Unknown architecture specified"
end repeat
