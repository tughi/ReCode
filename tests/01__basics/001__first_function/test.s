.globl main
main:
.L__main__S:
.L__main__1:
  # %1: i32 = const 0
  # ret %1
  movl $0x0, %eax
  jmp .L__main__E
.L__main__E:
  ret
