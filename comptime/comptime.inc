include 'fmt.inc'

;; Assamble line and print
macro TRACE line*
 {
  FMT 'TRACE ' __FILE__ '[' %D __LINE__ ']: '
  FMT_EXPR <line>
  line
 }

;; Assamble line
macro NO_TRACE line* { line }

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

;; Parse compilation target
if ~ defined RELEASE
  _RELEASE = 0 ;; default -> debug mode
else
  _RELEASE = RELEASE
end if

; Mark code block as debug block
; that can be excluded from release build.
; Debug mode is default.
; To compile in release mode: fasm -d RELEASE=1 code.asm
macro DEBUG { if _RELEASE = 0 }
macro END_DEBUG { end if }

;;
;; Target formats:
;;
macro FORMAT_ELF64_OBJECT
 {
  format ELF64
  BUILD_TYPE equ 'OBJECT'
  __MAIN_FILE__ equ __FILE__
 }

macro FORMAT_ELF64_EXECUTABLE
 {
  format ELF64 executable 3
  BUILD_TYPE equ 'EXECUTABLE'
  __MAIN_FILE__ equ __FILE__
 }

;;
;; Target agnostic entry point
;;
macro MAIN entry_point*
 {
  repeat 1
  match symbol:, entry_point \{
    match 'OBJECT', BUILD_TYPE \\{
      public symbol
    \\}
    match 'EXECUTABLE', BUILD_TYPE \\{
      entry symbol
    \\}
    symbol:
  break
  \}
  STATIC_ASSERT 0, "Usage: MAIN <symbol>: \n "
  end repeat
 }

;;
;; Macro that includes file with implementation
;;
macro IMPL file*, prefix
 {
  include file
  EXPORT_API prefix
  __IMPLEMENTATION__
 }

;;
;; Will be implemented only in DEBUG mode
;;
macro IMPL_FOR_DEBUG file*, prefix
 {
  DEBUG
  IMPL file, prefix
  END_DEBUG
 }

;;
;; Facilitate building *.inc library files
;;
macro IMPL_SELF prefix
 {
  EXPORT_API prefix
  __IMPLEMENTATION__
 }

macro IMPL_IF_MAIN_FILE format*
 {
  STATIC_ASSERT `format eq 'ELF64_OBJECT' | `format eq 'ELF64_EXECUTABLE', \
      "Only OBJECT/EXECUTABLE allowed! given: ", `format
  postpone \{
  if ~ defined __MAIN_FILE__
      FORMAT_\#format
      IMPL_SELF
  end if
  \}
}

macro EXPORT_API prefix
 {
  if BUILD_TYPE eq 'OBJECT'
  irpv symbol, __SYMBOLS__
  \{
    if ~ prefix eq
      label prefix\#symbol at symbol
      public prefix\#symbol
    else
      public symbol
    end if
  \}
  end if
 }

;;
;; Includes file and adds extern symbols
;; when compiling an object
;;
macro USE file*, prefix
 {
  include file
  IMPORT_API prefix
 }

macro IMPORT_API prefix
 {
  if BUILD_TYPE eq 'OBJECT'
    irpv symbol, __SYMBOLS__
    \{
      if ~ prefix eq
        extrn prefix\#symbol
      else
        extrn symbol
      end if
    \}
  end if
 }

;;
;; Will be used only in DEBUG mode
;;
macro USE_FOR_DEBUG file*, prefix
 {
  DEBUG
  USE file, prefix
  END_DEBUG
 }


;;
;; Pushes symbols into an API list
;;
macro API symbol*
 {
  __SYMBOLS__ equ symbol
 }

;;
;; Target agnostic segment/section marking
;;
macro CODE_SEGMENT
 {
  STATIC_ASSERT defined BUILD_TYPE, "BUILD_TYPE should be defiend!"
  if BUILD_TYPE eq 'EXECUTABLE'
    segment readable executable
  else
    section '.text' executable
  end if
 }

macro RODATA_SEGMENT
 {
  STATIC_ASSERT defined BUILD_TYPE, "BUILD_TYPE should be defiend!"
  if BUILD_TYPE eq 'EXECUTABLE'
    segment readable
  else
    section '.rodata' executable
  end if
 }

 macro DATA_SEGMENT
 {
  STATIC_ASSERT defined BUILD_TYPE, "BUILD_TYPE should be defiend!"
  if BUILD_TYPE eq 'EXECUTABLE'
    segment readable writable
  else
    section '.data' writable
  end if
 }

 macro BSS_SEGMENT
 {
  STATIC_ASSERT defined BUILD_TYPE, "BUILD_TYPE should be defiend!"
  if BUILD_TYPE eq 'EXECUTABLE'
    segment readable writable
  else
    section '.bss' writable
  end if
 }

