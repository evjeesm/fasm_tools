if ~ file$signal.inc eq 1
    err ;; Requires signal.inc
end if

; Register indices
REG_R8      =  0
REG_R9      =  1
REG_R10     =  2
REG_R11     =  3
REG_R12     =  4
REG_R13     =  5
REG_R14     =  6
REG_R15     =  7
REG_RDI     =  8
REG_RSI     =  9
REG_RBP     = 10
REG_RBX     = 11
REG_RDX     = 12
REG_RAX     = 13
REG_RCX     = 14
REG_RSP     = 15
REG_RIP     = 16
REG_EFL     = 17
REG_CSGSFS  = 18 ; Actually short cs, gs, fs, __pad0.
REG_ERR     = 19
REG_TRAPNO  = 20
REG_OLDMASK = 21
REG_CR2     = 22
NGREG       = 23


struc FPXREG
 {
.significand rw 4
.exponent    rw 1
.reserved    rw 3
 }

struc XMMREG ; 128bit
 {
.l rq 1
.h rq 1
 }

struc FPSTATE_T
 {
 ; 64-bit FXSAVE format.
.cwd       rw 1
.swd       rw 1
.ftw       rw 1
.fop       rw 1
.rip       rq 1
.rdp       rq 1
.mxcsr     rd 1
.mxcr_mask rd 1
.st        rq 2 * 8  ; FPXREG x8
.xmm       rq 2 * 16 ; XMMREG x16
.reserved  rd 24
 }

; stack sizes
MINSIGSTKSZ = 2048
SIGSTKSZ    = 8192 ; system default

; ss_flags
SS_ONSTACK  = 1
SS_DISABLED = 2

struc STACK_T
 {
.ss_sp      rq 1
.ss_flags   rd 1
._pad       rd 1
.ss_size    rq 1
 }

struc MCONTEXT_T
 {
.gregs       rq NGREG     ; general purpose regs
.fpregs      rq 1         ; FPSTATE_T ptr
.reserved    rq 8
 }

struc UCONTEXT_T
 {
.uc_flags    rq 1
.uc_link     rq 1       ; UCONTEXT_T ptr
.uc_stack    STACK_T
.uc_mcontext MCONTEXT_T
.uc_sigmask  SIGSET_T
._pad rb (SIGSET_RESERVED - SIGSET_SIZE)
._fp_regs    FPSTATE_T
._ssp        rq 4
 }


