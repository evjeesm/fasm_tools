purge IOTA_SET_STRIDE
macro IOTA_SET_STRIDE value:1
 { IOTA_stride = value }

purge IOTA_COUNTER_FUNC
macro IOTA_COUNTER_FUNC
 { IOTA_count = IOTA_count + IOTA_stride }

purge IOTA_STRIDE_FUNC
macro IOTA_STRIDE_FUNC
 { IOTA_stride = IOTA_stride }

purge IOTA
macro IOTA expr*
 {
  if ~ definite IOTA_count
    IOTA_count = 0
  end if
  if ~ definite IOTA_stride
    IOTA_stride = 1
  end if
  while 1
  match a == b, expr
  \{
    a = b
    IOTA_count = a + 1
    break
  \}
  match a, expr
  \{
    a = IOTA_count
    break
  \}
  break
  end while
  IOTA_COUNTER_FUNC
  IOTA_STRIDE_FUNC
 }
