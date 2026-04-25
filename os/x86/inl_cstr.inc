
;;
purge CSTR_LEN
macro CSTR_LEN reg*, cstr*
 {
  push edi
  xor eax, eax
  lea edi, cstr
  mov ecx, -1
  cld
  repnz scasb
  not ecx
  dec ecx
  mov reg, ecx
  pop edi
 }
