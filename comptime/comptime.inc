if ~ definite _COMPTIME_INC_
_COMPTIME_INC_ equ 1

include 'build.inc'

;;
;; Classical compile time assert with message.
;;
macro STATIC_ASSERT cond*, [msg*]
 {
 common
  if ~(cond)
    display \
      "[31m", \
      "STATIC ASSERT: ", msg, \
      "[0m"
    err
  end if
 }

macro REQUIRE [include_tag*]
 {
 forward
  STATIC_ASSERT definite include_tag, 'Dependency "', `include_tag, '" is missing!'
 }

macro DISP_LINE_NUM line*
 {
  local orig,n,dv,dg
  orig = line
  n = orig
  dv = 1
  while n > 0
    dv = dv*10
    n = n/10
  end while
  n = orig
  while dv > 1
    dv = dv/10
    dg = '0' + ((n/dv) mod 10)
    display dg
  end while
 }

;; Format code as it is.
;; Enclose in angular brackets
macro DISP_CODE_LINE [line*]
 {
 forward
  irps e, line
  \{
   display \`e, ' '
  \}
  display 10
 }

;; Assemble line and print.
;; Enclose in angular brackets
macro TRACE line*
 {
  display 'TRACE ',__FILE__,'['
  DISP_LINE_NUM __LINE__
  display ']: '
  DISP_CODE_LINE <line>
  line
 }

;; Assemble line
macro NO_TRACE line* { line }

 end if ;; _COMPTIME_INC_
