.globl main
main:
.L__main__S:
.L__main__1:
  # %1: i32 = const 255
  # %2: u8 = call $cast %1
  movl $0xff, %edi
  call cast
  # %3: u8 = const 255
  # %4: bool = cmp_ne %2 %3
  cmp $0xff, %al
  setne %cl
  # br %4 @2 @3
  cmp $0x0, %cl
  jne .L__main__2
  jmp .L__main__3
.L__main__2:
  # %5: i32 = const 1
  # ret %5
  movl $0x1, %eax
  jmp .L__main__E
.L__main__3:
  # %6: i32 = const -1
  # %7: u8 = call $cast %6
  movl $0xffffffff, %edi
  call cast
  # %8: u8 = const 255
  # %9: bool = cmp_ne %7 %8
  cmp $0xff, %al
  setne %cl
  # br %9 @4 @5
  cmp $0x0, %cl
  jne .L__main__4
  jmp .L__main__5
.L__main__4:
  # %10: i32 = const 2
  # ret %10
  movl $0x2, %eax
  jmp .L__main__E
.L__main__5:
  # %11: i32 = const -255
  # %12: u8 = call $cast %11
  movl $0xffffff01, %edi
  call cast
  # %13: u8 = const 1
  # %14: bool = cmp_ne %12 %13
  cmp $0x1, %al
  setne %cl
  # br %14 @6 @7
  cmp $0x0, %cl
  jne .L__main__6
  jmp .L__main__7
.L__main__6:
  # %15: i32 = const 3
  # ret %15
  movl $0x3, %eax
  jmp .L__main__E
.L__main__7:
  # %16: i32 = const 0
  # ret %16
  movl $0x0, %eax
  jmp .L__main__E
.L__main__E:
  ret


cast:
.L__cast__S:
  sub $0x10, %rsp
.L__cast__1:
  # %value.ptr: ptr<i32> = alloc i32
  # store %value.ptr %value
  movl %edi, 12(%rsp)
  # %value.1: i32 = load %value.ptr
  movl 12(%rsp), %eax
  # %1: u8 = cast %value.1
  movl %eax, %ecx
  # ret %1
  movb %cl, %al
  jmp .L__cast__E
.L__cast__E:
  add $0x10, %rsp
  ret