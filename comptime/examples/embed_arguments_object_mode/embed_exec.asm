include 'comptime.inc'
include 'elf64.inc'
include 'vtext.inc'
include 'args.inc'
include 'fmt.inc'

FORMAT_ELF64_EXECUTABLE

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
  load e_shentsize word from ehdr.e_shentsize
  load e_phentsize word from ehdr.e_phentsize
  load e_shnum word from ehdr.e_shnum
  if e_shnum >= SHN_LORESERVE
      err; Unimplemented
  end if
  load e_phnum word from ehdr.e_phnum
  if e_phnum >= SHN_LORESERVE
      err; Unimplemented
  end if
  load e_shstrndx word from ehdr.e_shstrndx
  if e_shstrndx >= SHN_LORESERVE
      err; Unimplemented
  end if

  FMT "@" %U ehdr.e_ident     " magic = " %S magic ' ' %0X . %NL
  FMT "@" %U ehdr.e_type      " e_type = " %X e_type %NL
  FMT "@" %U ehdr.e_machine   " e_machine = " %X e_machine %NL
  FMT "@" %U ehdr.e_entry     " e_entry = " %D e_entry %NL
  FMT "@" %U ehdr.e_phoff     " e_phoff = " %X e_phoff %NL
  FMT "@" %U ehdr.e_shoff     " e_shoff = " %X e_shoff %NL
  FMT "@" %U ehdr.e_shentsize " e_shentsize = " %D e_shentsize %NL
  FMT "@" %U ehdr.e_phentsize " e_phentsize = " %D e_phentsize %NL
  FMT "@" %U ehdr.e_ehsize    " e_ehsize = " %D e_ehsize %NL
end virtual

;; creating an entry label
CODE_SEGMENT
MAIN ..start:
    ;; Prepare for call
    PUSH_PTRS ..env0, ..env1
    PUSH_PTRS ..arg0, ..arg1, ..arg2
    push 3 ;; args count

    PRINT_INL "Embed runs!", 0xA
    PRINT_VALUE ..payload_entry
    ;; int3
if ~ PAYLOAD_CALL eq & PAYLOAD_CALL = 1
    call ..payload_entry ;; treat payload as a function
    PRINT_INL "=== After payload! ==="
    SYSCALL EXIT, 0
else
    jmp ..payload_entry ;; unconditionally pass controll to the payload
end if

DATA_SEGMENT
..arg0:
    db 'ARG 0', 0
..arg1:
    db 'ARG 1', 0
..arg2:
    db 'ARG 2', 0

..env0:
    db 'HOME="my home"', 0
..env1:
    db 'SHELL="my shell"', 0

;; Alignment
virtual at $
  align 4096
  pad = $ - $$
end virtual
db pad dup 0

origin = $ ;; this is new zero, here ..payload will reside
FMT "ORIGIN: " %U origin %NL

;; relative file offset
reloff = origin - 400000h
FMT "RELOFF: " %U reloff %NL

;; label ..payload_entry at reloff + e_entry - 0x1000;; + 0xe8

FMT "PAYLOAD ENTRY: " %0X ..payload_entry %NL

offset = 0

if e_shoff = 0
  display "no section header", 10
else
  display "Section Header:", 10

  ;; String table
  display " ===> String table:", 10
  offset = e_shoff + (e_shentsize * e_shstrndx)
  virtual at offset
    virtual at $
      shstrtab Elf64_Shdr
    end virtual
    VTEXT_BAKE _, ..payload, ASSEMBLE, offset+sizeof.Elf64_Shdr, offset
    load shstrtab_offset qword from shstrtab.sh_offset
    FMT "@" %U shstrtab.sh_offset " STRTAB offset = " %D shstrtab_offset %NL

    load shstrtab_size qword from shstrtab.sh_size
    FMT "@" %U shstrtab.sh_size " STRTAB size = " %D shstrtab_size %NL

    load shstrtab_entsize qword from shstrtab.sh_entsize
    FMT "@" %U shstrtab.sh_entsize " STRTAB entsize = " %D shstrtab_entsize %NL

    ;; access nth null-terminated string
    macro NTH_STR vsrc*, N*, _offset*, _max* {
      local count,char_count,char,offset
      count = 0
      char_count = 0
      offset = _offset
      max = _offset + _max
      while 1
         VTEXT_LOAD_CHAR vsrc, offset+char_count, char
         if char = 0
           count = count + 1
           if count >= N
             break
           else
             offset = offset + char_count
             char_count = 0
             STATIC_ASSERT offset < max, "Oooops!"
           end if
         end if
         char_count = char_count + 1
      end while
      VTEXT_DISPLAY vsrc, offset, offset+char_count
    }

    VTEXT_DISPLAY ..payload, shstrtab_offset, shstrtab_offset+shstrtab_size
  end virtual

  index = 0
  offset = e_shoff
  while index < e_shnum
    virtual at offset
      VTEXT_BAKE _, ..payload, ASSEMBLE, $$+e_shentsize, $$
      load sh_name dword from $$+_Elf64_Shdr_.sh_name
      FMT "@" %U $$+_Elf64_Shdr_.sh_name " sh_name = " %D sh_name %NL

      FMT "Section '"
      VTEXT_DISPLAY_NULLTERM ..payload, shstrtab_offset+sh_name
      FMT "' #" %U index " AT " %X $$ %NL

      load sh_type dword from $$+_Elf64_Shdr_.sh_type
      FMT "@" %U $$+_Elf64_Shdr_.sh_type " sh_type = " %D sh_type %NL
      ;; STATIC_ASSERT sh_type = SH_STRTAB , "Unsupported section type"
      if sh_type = SHT_SYMTAB
          FMT "TODO: List symbols..." %NL
      end if

      load sh_addr qword from $$+_Elf64_Shdr_.sh_addr
      FMT "@" %U $$+_Elf64_Shdr_.sh_addr " sh_addr = " %D sh_addr %NL

      load sh_offset qword from $$+_Elf64_Shdr_.sh_offset
      FMT "@" %U $$+_Elf64_Shdr_.sh_offset " sh_offset = " %D sh_offset %NL

      load sh_entsize qword from $$+_Elf64_Shdr_.sh_entsize
      FMT "@" %U $$+_Elf64_Shdr_.sh_entsize " sh_entsize = " %D sh_entsize %NL

    end virtual
    offset = offset + e_shentsize
    index = index + 1
  end while
end if

if e_phoff <> 0 ;; Program header
  FMT "Program header:"  %NL
  ;; Iterating over program header entries
  index = 0
  offset = e_phoff
  while index < e_phnum
    virtual at offset
      VTEXT_BAKE _, ..payload, ASSEMBLE, $$+sizeof.Elf64_Phdr, $$
      FMT "Segment #" %U index " AT " %X $$ %NL

      load p_type dword from $$+_Elf64_Phdr_.p_type
      FMT "@" %U $$+_Elf64_Phdr_.p_type " p_type = " %D p_type %NL
      STATIC_ASSERT p_type = PT_LOAD , "Unsupported segment type"

      load p_offset qword from $$+_Elf64_Phdr_.p_offset
      FMT "@" %U $$+_Elf64_Phdr_.p_offset " p_offset = " %D p_offset %NL

      load p_vaddr dword from $$+_Elf64_Phdr_.p_vaddr
      FMT "@" %U $$+_Elf64_Phdr_.p_vaddr " p_vaddr = " %D p_vaddr %NL

      load p_paddr dword from $$+_Elf64_Phdr_.p_paddr
      FMT "@" %U $$+_Elf64_Phdr_.p_paddr " p_paddr = " %D p_paddr %NL

      load p_filesz dword from $$+_Elf64_Phdr_.p_filesz
      FMT "@" %U $$+_Elf64_Phdr_.p_filesz " p_filesz = " %D p_filesz %NL

      load p_memsz dword from $$+_Elf64_Phdr_.p_memsz
      FMT "@" %U $$+_Elf64_Phdr_.p_memsz " p_memsz = " %D p_memsz %NL

      load p_align dword from $$+_Elf64_Phdr_.p_align
      FMT "@" %U $$+_Elf64_Phdr_.p_align " p_align = " %D p_align %NL

      load p_flags dword from $$+_Elf64_Phdr_.p_flags
    end virtual
    if p_offset = 0 ;; Meta data program header
      ;; patch payload entry offset, because we are not going to include first "META" segment
      ;; containing elf header and other stuff.
      aligned_size = (p_memsz + p_align - 1) and (not (p_align - 1))
      e_entry = e_entry - aligned_size
    else
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
    end if
    offset = offset + e_phentsize
    index = index + 1
  end while
end if ;; Program header

;; Declare payload entry postfactum
label ..payload_entry at reloff + e_entry
