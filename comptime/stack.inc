if ~ definite _STACK_INC_
_STACK_INC_ equ 1

include 'comptime.inc'
include 'vtext.inc'

macro STACK stack*, value_size*, capacity:10
 {
  VTEXT_PREALLOC stack#.stack.tmp, value_size
  stack#.stack.value_size = value_size
  stack#.stack.cap = capacity
  stack#.stack.size = 0
  VTEXT_PREALLOC stack, value_size * capacity
  stack#.len = value_size * capacity
 }

macro STACK_GROW stack*
 {
  if stack#.stack.size = stack#.stack.cap
    VTEXT_EXPAND stack, stack#.stack.value_size * stack#.stack.cap
    stack#.stack.cap = stack#.stack.cap * 2
    stack#.len = stack#.stack.cap * stack#.stack.value_size
  end if
 }

macro STACK_PUSH stack*, [value*]
 {
 forward
  local offset
  offset = stack#.stack.size * stack#.stack.value_size
  VTEXT_SET stack#.stack.tmp, value
  VTEXT_WRITE stack, offset, stack#.stack.tmp
  stack#.stack.size = stack#.stack.size + 1
  STACK_GROW stack
 }

macro STACK_PUSH_VTEXT stack*, [vtext*]
 {
 forward
  local offset
  offset = stack#.stack.size * stack#.stack.value_size
  STATIC_ASSERT vtext#.len = stack#.stack.value_size, 'vtext size should equal to stack.value_size'
  VTEXT_WRITE stack, offset, vtext
  stack#.stack.size = stack#.stack.size + 1
  STACK_GROW stack
 }

macro STACK_PEEK stack*, voutput*
 {
  local offset
  STATIC_ASSERT stack#.stack.size > 0, 'Stack is empty'
  offset = (stack#.stack.size - 1) * stack#.stack.value_size
  if ~ defined voutput
    VTEXT_PREALLOC voutput, stack#.stack.value_size
  end if
  VTEXT_READ voutput, stack, offset, stack#.stack.value_size
 }

macro STACK_POP stack*
 {
  STATIC_ASSERT stack#.stack.size > 0, 'Stack is empty'
  stack#.stack.size = stack#.stack.size - 1
 }

macro STACK_CLEAR stack* { stack#.stack.size = 0 }

macro STACK_IS_EMPTY stack*, output*
 {
  if stack#.stack.size = 0
    output = 1
  else
    output = 0
  end if
 }

macro WHILE_NOT_EMPTY stack* { while stack#.stack.size }
macro END_WHILE_NOT_EMPTY    { end while         }

macro VTEXT_STRUCT_FIELD name*, field*, def*
 {
  local offset, end_offset
  offset = $
  def
  end_offset = $
  name#.#field#.offset = offset
  name#.#field#.size = end_offset - offset
 }

macro VTEXT_STRUCT name*, [args*]
 {
 common
  virtual at 0
  local b,offset,size
 forward
    match field=,def,args
    \{
      VTEXT_STRUCT_FIELD name, field, def
    \}
 common
    offset = 0
    size = $-$$
    if ~ defined name
      VTEXT_PREALLOC name, size
    else
      VTEXT_RESERVE name, size
    end if
    while offset < size
      load b byte from offset
      store byte b at name:offset
      offset = offset + 1
    end while
  end virtual
  name#.len = size
 }

end if ;; ~ definite _STACK_INC_
