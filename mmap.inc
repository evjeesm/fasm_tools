; rax
MMAP = 9
MUNMAP = 11

MMAP_FAILED = -1

; edx
PROT_0 = 0x00
PROT_R = 0x01
PROT_W = 0x02
PROT_X = 0x04

; r10d
MAP_FIXED    = 0x0010
MAP_SHARED   = 0x0001
MAP_PRIVATE  = 0x0002
MAP_ANON     = 0x0020
MAP_STACK    = 0x4000

DFLT_PROT  = PROT_R or PROT_W
DFLT_FLAGS = MAP_ANON or MAP_PRIVATE
