include 'descriptors.inc'
if definite ARCH

  if ARCH = ARCH_X32
    include 'x32/inl_io.inc'
    display "INCLUDE INL_IO X32", 10
  else if ARCH = ARCH_X64
    include 'x64/inl_io.inc'
    display "INCLUDE INL_IO X64", 10
  else
    err ;; Unknown architecture
  end if
else
  err ;; Unspecified architecture
end if
