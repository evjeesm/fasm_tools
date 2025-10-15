format ELF64 executable 3
include 'comptime.inc'
include 'elf64.inc'
include 'vtext.inc'

VTEXT_FILE ..payload, './payload'
;; Assembling only elf header
virtual at 0
  virtual at $
    ehdr Elf64_Ehdr
  end virtual
  VTEXT_BAKE _, ..payload, ASSEMBLE, sizeof.Elf64_Ehdr
  load magic dword from ehdr.e_ident
  load e_type word from ehdr.e_type
  load e_machine word from ehdr.e_machine

  STATIC_ASSERT magic = ELFMAG , "ELF signature is corrupted"
  STATIC_ASSERT e_type = ET_EXEC, "File is not executable"
  STATIC_ASSERT e_machine = EM_X86_64, "Foreign architecture"

  load e_entry qword from ehdr.e_entry
  load e_shoff qword from ehdr.e_shoff
  load e_phoff qword from ehdr.e_phoff
  load e_ehsize word from ehdr.e_ehsize
  load e_phentsize word from ehdr.e_phentsize
  load e_phnum word from ehdr.e_phnum

  FMT "@" %U ehdr.e_ident     " magic = " %S magic ' ' %0X . %NL
  FMT "@" %U ehdr.e_type      " e_type = " %X e_type %NL
  FMT "@" %U ehdr.e_machine   " e_machine = " %X e_machine %NL
  FMT "@" %U ehdr.e_entry     " e_entry = " %D e_entry %NL
  FMT "@" %U ehdr.e_phoff     " e_phoff = " %X e_phoff %NL
  FMT "@" %U ehdr.e_shoff     " e_shoff = " %X e_shoff %NL
  FMT "@" %U ehdr.e_phentsize " e_phentsize = " %D e_phentsize %NL
end virtual

;; creating an entry label
segment readable executable
entry ..start
..start:
    ;; int3
    mov rdi, 1
    lea rsi, [..buf]
    mov rdx, ..buf.len
    mov rax, 1
    syscall
  ;; int3
  jmp ..payload_entry

segment readable
..buf:
    db 'Hello, from embed!'
    db 0xA
..buf.len = $ - ..buf

;; Alignment
virtual at $
  align 4096
  pad = $ - $$
end virtual
db pad dup 0

origin = $ ;; this is new zero, here ..payload will reside
FMT "ORIGIN: " %U origin %NL
reloff = origin - 400000h
FMT "RELOFF: " %U reloff %NL

label ..payload_entry at reloff + e_entry

if e_shoff = 0
  display "no section header", 10
else
  err ;; Unimpl
end if
FMT "@" %U ehdr.e_ehsize " e_ehsize = " %D e_ehsize %NL

if e_phoff <> 0 ;; Program header
  FMT "Program header:"  %NL
  ;; Iterating over program header entries
  index = 0
  offset = e_phoff
  macro PH count*, [seg*]
   {
   forward
    if index < count
      virtual at offset
        virtual at $
          seg Elf64_Phdr
        end virtual
        VTEXT_BAKE _, ..payload, ASSEMBLE, offset+sizeof.Elf64_Phdr, offset

        FMT "Segment #" %U index " AT " %X seg %NL
        load p_type dword from seg#.p_type
        FMT "@" %U seg#.p_type " p_type = " %D p_type %NL
        STATIC_ASSERT p_type = PT_LOAD , "Unsupported section type"

        load p_offset qword from seg#.p_offset
        FMT "@" %U seg#.p_offset " p_offset = " %D p_offset %NL

        load p_vaddr dword from seg#.p_vaddr
        FMT "@" %U seg#.p_vaddr " p_vaddr = " %D p_vaddr %NL

        load p_paddr dword from seg#.p_paddr
        FMT "@" %U seg#.p_paddr " p_paddr = " %D p_paddr %NL

        load p_filesz dword from seg#.p_filesz
        FMT "@" %U seg#.p_filesz " p_filesz = " %D p_filesz %NL

        load p_memsz dword from seg#.p_memsz
        FMT "@" %U seg#.p_memsz " p_memsz = " %D p_memsz %NL

        load p_align dword from seg#.p_align
        FMT "@" %U seg#.p_align " p_align = " %D p_align %NL

        load p_flags dword from seg#.p_flags
      end virtual
      ;; Here is where segment is built
      if p_flags and PF_X & p_flags and PF_R
        FMT "executable readable"
        ;; align p_align
        segment readable executable
      else if p_flags and PF_R & p_flags and PF_W
        FMT "readable writable"
        ;; align p_align
        segment readable writable
      else if p_flags and PF_R
        FMT "readable"
        ;; align p_align
        segment readable
      else
        err ;; unsupported segment
      end if
      FMT %NL
      VTEXT_BAKE _, ..payload, ASSEMBLE, p_offset + p_filesz, p_offset
      offset = offset + e_phentsize
      index = index + 1
    end if
   }
  PH e_phnum, a, b, c, d, e, f
end if ;; Program header

