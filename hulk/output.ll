; ModuleID = 'hulk_module'
source_filename = "hulk_module"

@0 = private unnamed_addr constant [23 x i8] c"The meaning of life is\00", align 1
@1 = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@2 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@3 = private unnamed_addr constant [3 x i8] c"%d\00", align 1

define i32 @main() {
entry:
  %text = alloca ptr, align 8
  %number = alloca i32, align 4
  store i32 42, ptr %number, align 4
  store ptr @0, ptr %text, align 8
  %text1 = load ptr, ptr %text, align 8
  %number2 = load i32, ptr %number, align 4
  %concat_heap_buf = call ptr @malloc(i64 256)
  %0 = call ptr @strcpy(ptr %concat_heap_buf, ptr %text1)
  %temp_heap_buf = call ptr @malloc(i64 32)
  %1 = call i32 (ptr, ptr, ...) @sprintf(ptr %temp_heap_buf, ptr @1, i32 %number2)
  %2 = call ptr @strcat(ptr %concat_heap_buf, ptr %temp_heap_buf)
  call void @free(ptr %temp_heap_buf)
  %3 = call i32 (ptr, ...) @printf(ptr @2, ptr %concat_heap_buf)
  %text3 = load ptr, ptr %text, align 8
  %number4 = load i32, ptr %number, align 4
  %concat_heap_buf5 = call ptr @malloc(i64 256)
  %4 = call ptr @strcpy(ptr %concat_heap_buf5, ptr %text3)
  %temp_heap_buf6 = call ptr @malloc(i64 32)
  %5 = call i32 (ptr, ptr, ...) @sprintf(ptr %temp_heap_buf6, ptr @3, i32 %number4)
  %6 = call ptr @strcat(ptr %concat_heap_buf5, ptr %temp_heap_buf6)
  call void @free(ptr %temp_heap_buf6)
  ret i32 0
}

declare ptr @malloc(i64)

declare i32 @sprintf(ptr, ptr, ...)

declare ptr @strcpy(ptr, ptr)

declare ptr @strcat(ptr, ptr)

declare void @free(ptr)

declare i32 @printf(ptr, ...)
