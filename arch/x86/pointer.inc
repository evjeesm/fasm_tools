include 'comptime/comptime.inc'

;; Assing rip-relative pointer
restruc REL_PTR
struc REL_PTR [symbol*]
 {
 forward
  dd symbol - $
 }

;; Convert rip-relative pointer to absolute
purge REL_TO_ABS
macro REL_TO_ABS reg*, relptr*
 {
 common
  TRACE <lea eax, [relptr]> ;; base
  TRACE <mov reg, [eax]>
  TRACE <add reg, eax>
 }

;; Multiple dereference of regular pointer
purge DEREF
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
purge DEREF_REL
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
