include 'descriptors.inc'
STATIC_ASSERT definite ARCH, "ARCH should be defined!"

if ARCH = ARCH_X86
  include 'x86/inl_io.inc'
  display "INCLUDE INL_IO X86", 10
else if ARCH = ARCH_X86_64
  include 'x86_64/inl_io.inc'
  display "INCLUDE INL_IO X86_64", 10
else
  err ;; Unknown architecture
end if

repeat 1
match =ARCH_X86, ARCH {
  include 'x86/inl_io.inc'
  DEBUG
    display "INCLUDE INL_IO X86", 10
  END_DEBUG
  break
}
match =ARCH_X86_64, ARCH {
  include 'x86_64/inl_io.inc'
  DEBUG
    display "INCLUDE INL_IO X86_64", 10
  END_DEBUG
  break
}
CRITICAL "Unknown architecture specified"
end repeat
