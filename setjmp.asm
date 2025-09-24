include 'comptime.inc' ;; requied
FORMAT_ELF64_OBJECT    ;; sets BUILD_TYPE

;; Implementation of the setjmp embedded right here
IMPL 'setjmp.inc'
