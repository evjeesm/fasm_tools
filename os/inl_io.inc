include 'descriptors.inc'
if definite ARCH

  if ARCH = ARCH_X86
    include 'x86/inl_io.inc'
    display "INCLUDE INL_IO X86", 10
  else if ARCH = ARCH_X86_64
    include 'x86_64/inl_io.inc'
    display "INCLUDE INL_IO X86_64", 10
  else
    err ;; Unknown architecture
  end if
else
  err ;; Unspecified architecture
end if
