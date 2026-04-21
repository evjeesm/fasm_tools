include 'comptime.inc'

;; Assing rip-relative pointer
struc REL_PTR [symbol*]
 {
 forward
    dq symbol - $
 }

;; Convert rip-relative pointer to absolute
macro REL_TO_ABS reg*, relptr*
 {
 common
    TRACE <lea rax, [relptr]> ;; base
    TRACE <mov reg, [rax]>
    TRACE <add reg, rax>
 }

;; Multiple dereference of regular pointer
macro DEREF reg*, pointer*
 {
   repeat 1
   match [loc], pointer
   \{
     TRACE <lea reg, [loc]>
   break
   \}
   match stars[loc], pointer
   \{
     TRACE <lea reg, [loc]>
     irps _, stars
     \\{
       TRACE <mov reg, [reg]>
     \\}
   break
   \}
   end repeat
 }

;; Multiple dereference of rip-relative pointer
macro DEREF_REL reg*, pointer*
 {
   repeat 1
   match [loc], pointer
   \{
     REL_TO_ABS reg, loc
   break
   \}
   match stars[loc], pointer
   \{
     REL_TO_ABS reg, loc
     irps _, stars
     \\{
       REL_TO_ABS reg, reg
     \\}
   break
   \}
   end repeat
 }
