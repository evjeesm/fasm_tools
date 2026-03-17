if ~ definite _ELF64_INC_
_ELF64 equ 1
;; parsing elf format functionality

;; uint16_t   Elf64_Half;
;; uint32_t   Elf64_Word;
;; int32_t    Elf64_Sword;
;; uint64_t   Elf64_Xword;
;; int64_t    Elf64_Sxword;
;; uint64_t   Elf64_Addr;
;; uint64_t   Elf64_Off;
;; uint16_t   Elf64_Section;
;; Elf64_Half Elf64_Versym;

;; The ELF file header.  This appears at the start of every ELF file.

SHN_LORESERVE equ 0xff00
SHN_XINDEX equ 0xffff
SHN_UNDEF equ 0
SHT_SYMTAB equ 2
SHT_STRTAB equ 3


ELFMAG equ 0x464c_457f

EI_NIDENT equ 16

struc Elf64_Ehdr
 {
  .e_ident       db EI_NIDENT dup ? ;; Magic number and other info
  .e_type        dw ?    ;; Object file type
  .e_machine     dw ?    ;; Architecture
  .e_version     dd ?    ;; Object file version
  .e_entry       dq ?    ;; Entry point virtual address
  .e_phoff       dq ?    ;; Program header table file offset
  .e_shoff       dq ?    ;; Section header table file offset
  .e_flags       dd ?    ;; Processor-specific flags
  .e_ehsize      dw ?    ;; ELF header size in bytes
  .e_phentsize   dw ?    ;; Program header table entry size
  .e_phnum       dw ?    ;; Program header table entry count
  .e_shentsize   dw ?    ;; Section header table entry size
  .e_shnum       dw ?    ;; Section header table entry count
  .e_shstrndx    dw ?    ;; Section header string table index
 }
virtual at 0
  _Elf64_Ehdr_ Elf64_Ehdr
  sizeof.Elf64_Ehdr = $
end virtual

;; Object file types
ET_EXEC equ 2
;;...

;; Supported machines
EM_X86_64 equ 62
;; EM_ARM equ 40
;;...

;; Section header.
struc Elf64_Shdr
 {
  .sh_name       dd ?    ;; Section name (string tbl index)
  .sh_type       dd ?    ;; Section type
  .sh_flags      dq ?    ;; Section flags
  .sh_addr       dq ?    ;; Section virtual addr at execution
  .sh_offset     dq ?    ;; Section file offset
  .sh_size       dq ?    ;; Section size in bytes
  .sh_link       dd ?    ;; Link to another section
  .sh_info       dd ?    ;; Additional section information
  .sh_addralign  dq ?    ;; Section alignment
  .sh_entsize    dq ?    ;; Entry size if section holds table
 }
virtual at 0
  _Elf64_Shdr_ Elf64_Shdr
  sizeof.Elf64_Shdr = $
end virtual

;; Symbol table entry.
struc Elf64_Sym
 {
  .st_name       dd ?    ;; Symbol name (string tbl index)
  .st_info       db ?    ;; Symbol type and binding
  .st_other      db ?    ;; Symbol visibility
  .st_shndx      dw ?    ;; Section index
  .st_value      dq ?    ;; Symbol value
  .st_size       dq ?    ;; Symbol size
 }
virtual at 0
  _Elf64_Sym_ Elf64_Sym
  sizeof.Elf64_Sym = $
end virtual

;; Relocation table entry without addend (in section of type SHT_REL).
struc Elf64_Rel
 {
  .r_offset      dq ?    ;; Address
  .r_info        dq ?    ;; Relocation type and symbol index
 }
virtual at 0
  _Elf64_Rel_ Elf64_Rel
  sizeof.Elf64_Rel = $
end virtual

;; Relocation table entry with addend (in section of type SHT_RELA).
struc Elf64_Rela
 {
  .r_offset      dq ?    ;; Address
  .r_info        dq ?    ;; Relocation type and symbol index
  .r_addend      dq ?    ;; Addend (signed)
 }
virtual at 0
  _Elf64_Rela_ Elf64_Rela
  sizeof.Elf64_Rela = $
end virtual

;; Program segment header.
struc Elf64_Phdr
 {
  .p_type        dd ?    ;; Segment type
  .p_flags       dd ?    ;; Segment flags
  .p_offset      dq ?    ;; Segment file offset
  .p_vaddr       dq ?    ;; Segment virtual address
  .p_paddr       dq ?    ;; Segment physical address
  .p_filesz      dq ?    ;; Segment size in file
  .p_memsz       dq ?    ;; Segment size in memory
  .p_align       dq ?    ;; Segment alignment
 }
virtual at 0
  _Elf64_Phdr_ Elf64_Phdr
  sizeof.Elf64_Phdr = $
end virtual

;; Legal values for p_type (segment type).
PT_LOAD equ 1   ;; Loadable program segment
;...

;; Legal values for p_flags (segment flags).
PF_X equ (1 shl 0)  ;; Segment is executable
PF_W equ (1 shl 1)  ;; Segment is writable
PF_R equ (1 shl 2)  ;; Segment is readable

;; Dynamic section entry.
struc Elf64_Dyn
 {
  .d_tag         dq ?    ;; Dynamic entry type
  .d_un          dq ?    ;; union
  label .d_val qword at .d_un
  label .d_ptr qword at .d_un
 }
virtual at 0
  _Elf64_Dyn_ Elf64_Dyn
  sizeof.Elf64_Dyn = $
end virtual

end if ;; _ELF64_INC_
