purge CALL
macro CALL func*, [args]
 {
 ;; args are passed via stack solely
 reverse
  push args
 common
  call func
 }
