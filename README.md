# FASM TOOLS


---

- [ ] Compile time
  * [x] STATIC_ASSERT cond, msg

  * [x] FMT - formatted copile time output
    - [x] '' String literal
    - [x] %D Signed integer   (decimal)
    - [x] %U Unsigned integer (decimal)
    - [x] %X Unsigned integer (hex)
    - [x] %S Interpret numeric constant as a String
    - [x] %F Floating point/scientific notation
       %P Precition for floating point number
    - [ ] %E code expression as is

  * [X] DEBUG/RELEASE conditional code inclusion
     `fasm -d RELEASE=0` - enable debug mode
     `fasm -d RELEASE=1` - enable release mode
     `DEBUG` / `END_DEBUG` - mark code debug only

  - [X] Making Executable/Object from same source
    - [X] `FORMAT_ELF64_OBJECT` / `FORMAT_ELF64_EXECUTABLE` - Pick target
    - [X] `MAIN label:` - Target agnostic entry point
    - [X] `*_REGION` - Target agnostic region/section definitions
       `(CODE`|`RODATA`|`DATA`|`BSS`) - will produce correct region/section defs
    - [ ] Importing/Exporting
      * [X] `API` - Making import/export list
      * [X] Prefixing of the __API__ symbols
      * [X] `IMPL file[, prefix]` - Implement include file (embed)
      * [X] `USE file[, prefix]` - Use include file (include and import symbols)
      * [X] `*_FOR_DEBUG` - conditional use/implement













