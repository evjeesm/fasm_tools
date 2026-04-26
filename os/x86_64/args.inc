include 'inl_io.inc'

;; ![IMPORTANT]
;; Macros designed to be used in prologue of the entry point,
;; so use them before establishing a stack frame.
;; If you need to access to argc, argv, or envp outside of the main prologue
;; or in subsequent routine calls, propagate them as arguments.

;;
;; Read argc into a register
;;
purge LOAD_ARGC
macro LOAD_ARGC reg*, skip:0
 {
    mov reg, [rsp+skip]
 }

;;
;; Read argc into a register
;;
purge LOAD_ARGC_AFTER_PROLOG
macro LOAD_ARGC_AFTER_PROLOG reg* { LOAD_ARGC reg, 8 }

;;
;; Override argc
;;
purge SET_ARGC
macro SET_ARGC argc*
 {
    mov dword [rsp], argc
 }

;;
;; Override argv
;;
purge SET_ARGV
macro SET_ARGV addr*
 {
    lea rax, addr
    mov qword [rsp+8], rax
 }

;;
;; Loads argument array pointer into a register
;;
purge LOAD_ARGV
macro LOAD_ARGV reg*,skip:0
 {
    lea reg, [rsp+8+skip]
 }

;;
;; Loads environment pointer into a register
;;
purge LOAD_ENVP
macro LOAD_ENVP reg*, skip:0
 {
    LOAD_ARGC rax, skip
    inc rax ;; skip argc
    LOAD_ARGV reg, skip
    lea reg, [reg + rax*8] ;; skip argv
 }

;;
;; Loads environment pointer into a register
;;
purge LOAD_ENVP_AFTER_PROLOG
macro LOAD_ENVP_AFTER_PROLOG reg* 
 {
    LOAD_ARGC_AFTER_PROLOG rax
    inc rax ;; skip argc
    LOAD_ARGV_AFTER_PROLOG reg
    lea reg, [reg + rax*8] ;; skip argv
 }

;;
;; Count amount of env variables (Null-terminated list)
;;
purge ENVP_COUNT
macro ENVP_COUNT envp*, reg*
 {
    xor rax, rax
    mov rdi, envp
    mov rcx, -1
    cld
    repnz scasq
    not rcx
    dec rcx
    mov reg, rcx
 }

;;
;; Load N-th argument pointer into a register
;;
purge LOAD_NTH_ARG
macro LOAD_NTH_ARG reg*, argv*, n*
 {
  if n eqtype rax
    lea reg, [n*8]
    lea reg, [argv + reg]
  else if n relativeto 0
    lea reg, [argv + 8*n]
  end if
 }

;;
;; Load N-th env pointer into a register
;;
purge LOAD_NTH_ENV
macro LOAD_NTH_ENV reg*, envp*, n*
 {
  if n eqtype rax
    lea reg, [n*8]
    lea reg, [envp + reg]
  else if n relativeto 0
    lea reg, [envp + 8*n]
  end if
 }

;;
;; Replace N-th ARG with other pointer
;;
purge SET_NTH_ARG
macro SET_NTH_ARG argv*, n*, addr*
 {
    lea rax, [addr]
    LOAD_NTH_ARG r10, argv, n
    mov [r10], rax
 }

;;
;; Replace N-th ENV with other pointer
;;
purge SET_NTH_ENV
macro SET_NTH_ENV envp*, n*, addr*
 {
  if addr eq 0
    xor rax, rax
  else
    lea rax, [addr]
  end if
    LOAD_NTH_ENV r10, envp, n
    mov [r10], rax
 }

;;
;; Print entirety of argument list
;;
purge PRINT_ARGS
macro PRINT_ARGS argv*, sep:' ', term:0xA
 {
 local .next_arg, .exit
    xor r12, r12
.next_arg:
    LOAD_NTH_ARG rax, argv, r12
    mov rax, [rax]
test rax, rax
jz .exit
    PRINT_CSTR [rax]
    PRINT_INL sep
    inc r12
jmp .next_arg
.exit:
    PRINT_INL term
 }

;;
;; Print entirety of enviromnent variable list
;;
purge PRINT_ENV
macro PRINT_ENV envp*, sep:' ', term:0xA
 {
 local .next_env, .exit
    mov r12, 0
.next_env:
    LOAD_NTH_ENV rax, envp, r12
    mov rax, [rax]
test rax, rax
jz .exit
    PRINT_CSTR [rax]
    PRINT_INL sep
    inc r12
jmp .next_env
.exit:
    PRINT_INL term
 }


;;
;; Make null ternimated list of pointers on the stack,
;; elements are placed in stack order (first will be placed on top)
purge PUSH_PTRS
macro PUSH_PTRS [args*]
 {
 common
    push 0 ;; null-separator
 reverse
    lea rax, [args]
    push rax
 common
 }
