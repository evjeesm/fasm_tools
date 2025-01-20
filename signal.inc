include 'ucontext.inc'

RT_SIGACTION = 13         ; modern sigaction syscall
SIGACTION = RT_SIGACTION  ; alias

RT_SIGPROCMASK = 14          ; syscall, access or change signal mask of a process.
SIGPROCMASK = RT_SIGPROCMASK ; alias

RT_SIGRETURN = 15         ;
SIGRETURN = RT_SIGRETURN  ; alias
SIGSET_SIZE = 128         ; size of the sigset on x86_64
                          ; (including real time signals)

POSIX_RTSIG_MAX = 8 ; from limits.h:
                    ; The number of realtime signal numbers
                    ; reserved for application use

; Default values used to initialize sigrtmin and sigrtmax libc variables
; that used in __libc_allocate_rtsig
SIGRTMIN = 32       ; highest priority
SIGRTMAX = 64       ; lowest priority


SA_NOCLDSTOP = 1          ; dont send SIGCHLD when children stop.
SA_NOCLDWAIT = 2          ; dont create zombie process on child death.
SA_SIGINFO   = 4          ; invoke 3-arg handler.
SA_RESTORER  = 0x04000000 ; use restorer trampoline
SA_ONSTACK   = 0x08000000 ; use signal stack by using `sa_restorer'.
SA_RESTART   = 0x10000000 ; restart syscall that was interrupted.
SA_NODEFER   = 0x40000000 ; dont auto-block signal that was being handled.
SA_RESETHAND = 0x80000000 ; oneshot action (reset to SIG_DFL).

; ISO C99 signals.
SIGINT  = 2      ; Interactive attention signal.
SIGILL  = 4      ; Illegal instruction.
SIGABRT = 6      ; Abnormal termination.
SIGFPE  = 8      ; Erroneous arithmetic operation.
SIGSEGV = 11     ; Invalid access to storage.
SIGTERM = 15     ; Termination request.

; Historical signals specified by POSIX.
SIGHUP  = 1      ; Hangup.
SIGQUIT = 3      ; Quit.
SIGTRAP = 5      ; Trace/breakpoint trap.
SIGKILL = 9      ; Killed.
SIGPIPE = 13     ; Broken pipe.
SIGALRM = 14     ; Alarm clock.

; Archaic names for compatibility.
SIGIO   = SIGPOLL ; I/O now possible (4.2 BSD).
SIGIOT  = SIGABRT ; IOT instruction, abort() on a PDP-11.
SIGCLD  = SIGCHLD ; Old System V name

; Historical signals specified by POSIX.
SIGBUS  =  7      ; Bus error.
SIGSYS  = 31      ; Bad system call.

; New(er) POSIX signals (1003.1-2008, 1003.1-2013).
SIGURG  = 23      ; Urgent data is available at a socket.
SIGSTOP = 19      ; Stop, unblockable.
SIGTSTP = 20      ; Keyboard stop.
SIGCONT = 18      ; Continue.
SIGCHLD = 17      ; Child terminated or stopped.
SIGTTIN = 21      ; Background read from control terminal.
SIGTTOU = 22      ; Background write to control terminal.
SIGPOLL = 29      ; Pollable event occurred (System V).
SIGXFSZ = 25      ; File size limit exceeded.
SIGXCPU = 24      ; CPU time limit exceeded.
SIGVTALRM = 26    ; Virtual timer expired.
SIGPROF = 27      ; Profiling timer expired.
SIGUSR1 = 10      ; User-defined signal 1.
SIGUSR2 = 12      ; User-defined signal 2.

; Nonstandard signals found in all modern POSIX systems
;  (including both BSD and Linux).
SIGWINCH = 28 ; Window size change (4.3 BSD, Sun).

SIGPWR = 30 ; Power failure imminent.

struc SIGACTION_T
 {
  common
.sa_sigaction   dq ?
.sa_flags       dd ?
._pad           dd ?
.sa_restorer    dq ?
.sa_mask    SIGSET_T
.size = $ - .sa_sigaction
 }

macro SIGACTION signal*, act*, oact:0
 {
  common
    mov rax, SIGACTION
    mov rdi, signal

    if ~ act eq 0
        lea rsi, [act]
    else
        xor rsi, rsi
    end if

    if ~ oact eq 0
        lea rdx, [oact]
    else
        xor rdx, rdx
    end if

    mov r10, POSIX_RTSIG_MAX
    syscall
 }
