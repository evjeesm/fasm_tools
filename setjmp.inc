
struc JMP_BUF
 {
common
.rbx rq 1
.rbp rq 1
.rsp rq 1
.r12 rq 1
.r13 rq 1
.r14 rq 1
.r15 rq 1
.rip rq 1
.sigmask_saved rw 1
.sigmask SIGSET_T
.size = $ - .rbx
 }
