;;
;; Classical compile time assert with message.
;;
purge STATIC_ASSERT
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

purge TODO
macro TODO [msg*]
 {
 common
  display \
    "[31m", \
    "Unimplemented: ", msg, \
    "[0m"
  err
 }

purge CRITICAL
macro CRITICAL [msg*]
 {
 common
  display \
    "[31m", \
    "Critical: ", msg, \
    "[0m"
  err
 }

purge INFO
macro INFO [msg*]
 {
 common
  display \
    "[36m", \
    "INFO: ", msg, \
    "[0m"
 }

purge DEBUG_MSG
macro DEBUG_MSG [msg*]
 {
 common
  DEBUG
    display \
      "[37m", \
      ": ", msg, \
      "[0m"
  END_DEBUG
 }

purge WARNING
macro WARNING [msg*]
 {
 common
  display \
    "[33m", \
    "Warning: ", msg, \
    "[0m"
 }

purge DISP_LINE_NUM
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
purge DISP_CODE_LINE
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
purge TRACE
macro TRACE line*
 {
  display 'TRACE ',__FILE__,'['
  DISP_LINE_NUM __LINE__
  display ']: '
  DISP_CODE_LINE <line>
  line
 }

purge DEBUG_TRACE
macro DEBUG_TRACE line*
 {
  DEBUG
    display 'TRACE ',__FILE__,'['
    DISP_LINE_NUM __LINE__
    display ']: '
    DISP_CODE_LINE <line>
  END_DEBUG
  line
 }
