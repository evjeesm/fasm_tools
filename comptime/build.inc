include 'comptime.inc'

;; Parse compilation target
if ~ defined RELEASE
  _RELEASE = 0 ;; default -> debug mode
else
  _RELEASE = RELEASE
end if

;; Mark code block as debug block
;; that can be excluded from release build.
;; Debug mode is default.
;; To compile in release mode: fasm -d RELEASE=1 code.asm
purge DEBUG
macro DEBUG { if _RELEASE = 0 }
purge END_DEBUG
macro END_DEBUG { end if }

;; using fix, so that 'match =ARCH_X86, ARCH {}' works
BUILD_TYPE_OBJECT fix 0
BUILD_TYPE_EXECUTABLE fix 1

ARCH_X86 fix 0
ARCH_X86_64 fix 1

;;
;; Target formats:
;;
purge TARGET
macro TARGET fmt*, type*
 {
  __MAIN_FILE__ equ __FILE__
  SET_FORMAT fmt, type
  display "TARGET: ", `fmt, ' ', `type, 0xA
 }

purge SET_FORMAT
macro SET_FORMAT fmt*, type*
 {
  repeat 1
  match =ELF64 =EXECUTABLE, fmt type
  \{
    format elf64 executable 3
    break
  \}
  match =ELF32 =EXECUTABLE, fmt type
  \{
    format elf executable 3
    break
  \}
  match =ELF64 =OBJECT, fmt type
  \{
    format elf64
    break
  \}
  match =ELF32 =OBJECT, fmt type
  \{
    format elf
    break
  \}
  err ;; wrong format
  end repeat

  ;; Set build type
  match =EXECUTABLE, type \{ BUILD_TYPE equ BUILD_TYPE_EXECUTABLE \}
  match =OBJECT, type     \{ BUILD_TYPE equ BUILD_TYPE_OBJECT     \}

  ;; Set arch
  match =ELF32, fmt \{ ARCH equ ARCH_X86 \}
  match =ELF64, fmt \{ ARCH equ ARCH_X86_64 \}

  ;; Include architecture related headers
  include '../arch/arch.inc'
 }

;;
;; Target agnostic entry point
;;
purge MAIN
macro MAIN entry_point*
 {
  repeat 1
  match symbol:, entry_point \{
    if BUILD_TYPE = BUILD_TYPE_OBJECT
      public symbol
    else if BUILD_TYPE = BUILD_TYPE_EXECUTABLE
      entry symbol
    end if
    symbol:
  break
  \}
  STATIC_ASSERT 0, "Usage: MAIN <symbol>: \n "
  end repeat
 }

;;
;; Macro that includes file with implementation
;;
purge IMPL
macro IMPL file*, prefix
 {
  include file
  EXPORT_API prefix
  __IMPLEMENTATION__
 }

;;
;; Will be implemented only in DEBUG mode
;;
purge IMPL_FOR_DEBUG
macro IMPL_FOR_DEBUG file*, prefix
 {
  DEBUG
  IMPL file, prefix
  END_DEBUG
 }

;;
;; Compile standalone library file.
;;
purge IMPL_IF_MAIN_FILE
macro IMPL_IF_MAIN_FILE fmt*, type*
 {
  if ~ defined __MAIN_FILE__
    SET_FORMAT fmt, type
  end if
  postpone \{
  if ~ defined __MAIN_FILE__
    IMPL_SELF
  end if
  \}
 }

;;
;; Facilitate building *.inc standalone library files
;;
purge IMPL_SELF
macro IMPL_SELF prefix
 {
  EXPORT_API prefix
  __IMPLEMENTATION__
 }

purge EXPORT_API
macro EXPORT_API prefix
 {
  if BUILD_TYPE = BUILD_TYPE_OBJECT
    irpv symbol, __SYMBOLS__ \{
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
purge USE
macro USE file*, prefix
 {
  include file
  IMPORT_API prefix
 }

purge USE_SHARED
macro USE_SHARED file*, prefix
 {
  include file
  IMPORT_API_SHARED prefix
 }

purge IMPORT_API
macro IMPORT_API prefix
 {
  if BUILD_TYPE = BUILD_TYPE_OBJECT
    irpv symbol, __SYMBOLS__ \{
      if ~ prefix eq
        extrn prefix\#symbol
      else
        extrn symbol
      end if
    \}
  end if
 }

purge EXTRN_PLT
macro EXTRN_PLT sym*
 {
  extrn `sym as _#sym
  sym = PLT _#sym
 }

purge IMPORT_API_SHARED
macro IMPORT_API_SHARED prefix
 {
  if BUILD_TYPE = BUILD_TYPE_OBJECT
    irpv symbol, __SYMBOLS__ \{
      if ~ prefix eq
        ;; extrn prefix\#symbol as _\#prefix\#symbol
        ;; prefix\#symbol = PLT _\#prefix\#symbol
        EXTRN_PLT prefix\#symbol
      else
        ;; extrn symbol as _\#symbol
        ;; symbol = PLT _\#symbol
        EXTRN_PLT symbol
      end if
    \}
  end if
 }

;;
;; Will be used only in DEBUG mode
;;
purge USE_FOR_DEBUG
macro USE_FOR_DEBUG file*, prefix
 {
  DEBUG
  USE file, prefix
  END_DEBUG
 }

;;
;; Will be used only in DEBUG mode
;;
purge USE_SHARED_FOR_DEBUG
macro USE_SHARED_FOR_DEBUG file*, prefix
 {
  DEBUG
  USE_SHARED file, prefix
  END_DEBUG
 }

;;
;; Pushes symbols into an API list
;;
purge API
macro API symbol*
 {
  __SYMBOLS__ equ symbol
 }

;;
;; Target agnostic segment/section marking
;;
purge CODE_SEGMENT
macro CODE_SEGMENT
 {
  STATIC_ASSERT defined BUILD_TYPE, "BUILD_TYPE should be defiend!"
  if BUILD_TYPE = BUILD_TYPE_EXECUTABLE
    segment readable executable
  else
    section '.text' executable
  end if
 }

purge RODATA_SEGMENT
macro RODATA_SEGMENT
 {
  STATIC_ASSERT defined BUILD_TYPE, "BUILD_TYPE should be defiend!"
  if BUILD_TYPE = BUILD_TYPE_EXECUTABLE
    segment readable
  else
    section '.rodata' executable
  end if
 }

purge DATA_SEGMENT
macro DATA_SEGMENT
 {
  STATIC_ASSERT defined BUILD_TYPE, "BUILD_TYPE should be defiend!"
  if BUILD_TYPE = BUILD_TYPE_EXECUTABLE
    segment readable writable
  else
    section '.data' writable
  end if
 }

purge BSS_SEGMENT
macro BSS_SEGMENT
 {
  STATIC_ASSERT defined BUILD_TYPE, "BUILD_TYPE should be defiend!"
  if BUILD_TYPE = BUILD_TYPE_EXECUTABLE
    segment readable writable
  else
    section '.bss' writable
  end if
 }
