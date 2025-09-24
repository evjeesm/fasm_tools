macro IOTA_SET_STRIDE value:1
 { IOTA_stride = value }

macro IOTA_COUNTER_FUNC
 { IOTA_count = IOTA_count + IOTA_stride }

macro IOTA_STRIDE_FUNC
 { IOTA_stride = IOTA_stride }

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

