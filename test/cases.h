{
   .name = "simple printf",
   .compiles = true,
   .source =
      "int printf(const char*, ...);"
      "int main(void) {"
      "  printf(\"Hello World\\n\");"
      "}",
   .output = "Hello World\n",
},
{
   .name = "sizeof(array)",
   .compiles = true,
   .source =
      "int main(void) {"
      "  int arr[10];"
      "  return sizeof(arr);"
      "}",
   .ret_val = 40,
},
{
   .name = "arraylen(VLA)",
   .compiles = true,
   .source =
      "int printf(const char*, ...);"
      "unsigned get(void) { return 10; }"
      "int main(void) {"
      "  int arr[get()];"
      "  printf(\"%u\", arraylen(arr));"
      "}",
   .output = "10",
},
{
   .name = "typedef",
   .compiles = true,
   .source =
      "typedef unsigned byte uint_t;"
      "int main(void) {"
      "  return sizeof(uint_t);"
      "}",
   .ret_val = 1,
},
{
   .name = "for-array",
   .compiles = true,
   .source =
      "int printf(const char*, ...);"
      "int main(void) {"
      "  int arr[8];"
      "  for (unsigned i = 0; i < arraylen(arr); ++i) {"
      "     arr[i] = i;"
      "  }"
      "  for (unsigned i = 0; i < arraylen(arr); ++i) {"
      "     printf(\"arr[%u]=%u\", i, arr[i]);"
      "  }"
      "  return 42;"
      "}",
   .output =
      "arr[0]=0"
      "arr[1]=1"
      "arr[2]=2"
      "arr[3]=3"
      "arr[4]=4"
      "arr[5]=5"
      "arr[6]=6"
      "arr[7]=7",
   .ret_val = 42,
},
{
   .name = "multiply/divide",
   .compiles = true,
   .source =
      "int printf(const char*, ...);"
      "int main(void) {"
      "  int a = 42;"
      "  int b = 99;"
      "  printf(\"%d\", (a * b) + (b / a));"
      "}",
   .output = "4160",
   .ret_val = 0
},
{
   .name = "casting of integers",
   .compiles = true,
   .source =
      "int printf(const char*, ...);"
      "int main(void) {"
      "  int a = -1;"
      "  printf(\"%d\", (short)a);"
      "}",
   .output = "-1",
   .ret_val = 0
},
{
   .name = "nested for-loop",
   .compiles = true,
   .source =
      "int putchar(int);"
      "int main(void) {"
      "  for (int a = 0; a < 3; ++a) {"
      "     for (int b = 0; b < 5; ++b) {"
      "        putchar('.');"
      "     }"
      "     putchar(' ');"
      "  }"
      "}",
   .output =
      "..... "
      "..... "
      "..... ",
},
{
   .name = "modulo",
   .compiles = true,
   .source =
      "int printf(const char*, ...);"
      "int main(void) {"
      "  int a = 42;"
      "  int b = 22;"
      "  printf(\"%d\", a % b);"
      "}",
   .output = "20",
   .ret_val = 0
},
{
   .name = "sizeof(VLA)",
   .compiles = true,
   .source =
      "int printf(const char*, ...);"
      "unsigned get(void) { return 10; }"
      "int main(void) {"
      "  byte arr[get()];"
      "  printf(\"%u\", sizeof(arr));"
      "}",
   .output = "10",
},
{
   .name = "fibonacci",
   .compiles = true,
   .source = 
      "int fib(int n) {"
      "  return n <= 1 ? n : (fib(n - 1) + fib(n - 2));"
      "}"
      "int main(void) {"
      "  return fib(10);"
      "}",
   .ret_val = 55,
},
{
   .name = "pointer-arithmetic",
   .compiles = true,
   .source =
      "int printf(const char*, ...);"
      "int main(void) {"
      "  int* a = (int*)(sizeof(int) * 12);"
      "  int* b = (int*)(sizeof(int) * 10);"
      "  printf(\"%d\", a - b);"
      "}",
   .output = "2",
},
{
   .name = "enum",
   .compiles = true,
   .source =
      "enum values {"
      "  VAL_A,"
      "  VAL_B,"
      "};"
      "int main(void) {"
      "  return VAL_B;"
      "}",
   .ret_val = 1,
},
{
   .name = "extern-var",
   .compiles = true,
   .source =
      "extern int val;"
      "int val = 42;"
      "int main(void) { return val; }",
   .ret_val = 42,
},
{
   .name = "multiple definitions of f",
   .compiles = false,
   .source =
      "int f(void) { return 32; }"
      "int f(void) { return 42; }"
      "int main(void) { return f(); }",
},
{
   .name = "undefined type global variable",
   .compiles = false,
   .source =
      "undefined a;"
      "int main() {}",
},
{
   .name = "undefined type local variable",
   .compiles = false,
   .source =
      "int main() { undefined a; }",
},
{
   .name = "undefined type function",
   .compiles = false,
   .source =
      "undefined a();"
      "int main() {}",
},
{
   .name = "undefined type parameter",
   .compiles = false,
   .source =
      "int main(undefined a) {}"
},
{
   .name = "unnamed enum",
   .compiles = true,
   .source =
      "enum {"
      "  VAL_A,"
      "};"
      "enum {"
      "  VAL_B"
      "};"
      "int main(void) { return VAL_B; }",
},
{
   .name = "multiple definitions of enum",
   .compiles = false,
   .source =
      "enum A {};"
      "enum A {};"
      "int main(void) {}"
},
{
   .name = "multiple declarations of local variables",
   .compiles = false,
   .source =
      "int main(void) {"
      "  int a;"
      "  int a;"
      "}",
},
{
   .name = "declaration of variable+parameter with the same name",
   .compiles = false,
   .source =
      "int main(int argc) {"
      "  int argc;"
      "}"
},
{
   .name = "multiple parameters with the same name",
   .compiles = false,
   .source =
      "int main(int argc, int argc) {"
      "}"
},
{
   .name = "multi-variable declaration",
   .compiles = true,
   .source =
      "int main(void) {"
      "  int a, b;"
      "  a = 32;"
      "  b = 45;"
      "  return a + b;"
      "}",
   .ret_val = 77,
},
{
   .name = "declaration of int+array",
   .compiles = true,
   .source =
      "int main(void) {"
      "  int a[10], b;"
      "  b = arraylen(a);"
      "  return b;"
      "}",
   .ret_val = 10,
},
{
   .name = "multi-global-variable declaration",
   .compiles = true,
   .source =
      "int a = 13, b = 31;"
      "int main(void) {"
      "  return a + b;"
      "}",
   .ret_val = 44,
},
{
   .name = "global declaration of int+array",
   .compiles = true,
   .source =
      "int a[10], b;"
      "int main(void) {"
      "  return (sizeof(a) == (sizeof(int) * 10)) + (sizeof(b) == sizeof(int));"
      "}",
   .ret_val = 2,
},
{
   .name = "comma-expression",
   .compiles = true,
   .source =
      "int main(void) {"
      "  return 42, 3;"
      "}",
   .ret_val = 3,
},
{
   .name = "void-function",
   .compiles = true,
   .source =
      "int printf(const char*, ...);"
      "static void puti(int i) {"
      "  printf(\"%d\\n\", i);"
      "  return;"
      "}"
      "int main(void) {"
      "  puti(42);"
      "}",
   .output = "42\n",
},
{
   .name = "boolean and",
   .compiles = true,
   .source =
      "int main(void) {"
      "  int a = 3, b = 2;"
      "  return a && b;"
      "}",
   .ret_val = 1,
},
{
   .name = "boolean or",
   .compiles = true,
   .source =
      "int main(void) {"
      "  int a = 3, b = 1;"
      "  return a || b;"
      "}",
   .ret_val = 1,
},
{
   .name = "boolean not",
   .compiles = true,
   .source =
      "int main(void) {"
      "  int a = 1;"
      "  return !a;"
      "}",
   .ret_val = 0,
},
{
   .name = "typedef unnamed enum",
   .compiles = true,
   .source =
      "typedef enum {"
      "  VAL_A,"
      "  VAL_B,"
      "} e_t;"
      "int main(void) {"
      "  e_t e = VAL_B;"
      "  return e;"
      "}",
   .ret_val = 1,
},
{
   .name = "typedef enum",
   .compiles = true,
   .source =
      "enum E {"
      "  VAL_A,"
      "  VAL_B,"
      "};"
      "typedef enum E e_t;"
      "int main(void) {"
      "  e_t e = VAL_B;"
      "  return e;"
      "}",
   .ret_val = 1,
},
{
   .name = "sizeof(struct)",
   .compiles = true,
   .source =
      "struct A {"
      "  int a;"
      "  int b;"
      "};"
      "int main(void) {"
      "  struct A a;"
      "  return sizeof(a) / sizeof(int);"
      "}",
   .ret_val = 2,
},
{
   .name = "struct-member",
   .compiles = true,
   .source =
      "struct A {"
      "  int a;"
      "  int b;"
      "};"
      "int main(void) {"
      "  struct A a;"
      "  a.a = 42;"
      "  return a.a;"
      "}",
   .ret_val = 42,
},
{
   .name = "member-of-int",
   .compiles = false,
   .source =
      "int main(void) {"
      "  int a = 42;"
      "  return a.i;"
      "}"
},
{
   .name = "arrow-operator",
   .compiles = true,
   .source =
      "struct A { int a; int b; };"
      "int main(void) {"
      "  struct A a;"
      "  a.a = 42;"
      "  const struct A* b = &a;"
      "  return b->a;"
      "}",
   .ret_val = 42,
},
{
   .name = "typedef struct",
   .compiles = true,
   .source =
      "struct A { int a; int b; };"
      "typedef struct A A;"
      "int main(void) {"
      "  A a;"
      "  a.a = 42;"
      "  return a.a;"
      "}",
   .ret_val = 42,
},
{
   .name = "struct A & enum A",
   .compiles = true,
   .source =
      "enum A { VAL_A, VAL_B };"
      "struct A { enum A a; };"
      "int main(void) {"
      "  struct A a;"
      "  a.a = VAL_B;"
      "  return a.a;"
      "}",
   .ret_val = 1,
},
{
   .name = "struct in struct",
   .compiles = true,
   .source =
      "struct A {"
      "  struct {"
      "     int c;"
      "  } b;"
      "};"
      "int main(void) {"
      "  struct A a;"
      "  a.b.c = 42;"
      "  return a.b.c;"
      "}",
   .ret_val = 42,
},
{
   .name = "union",
   .compiles = true,
   .source =
      "union A {"
      "  int a;"
      "  int b;"
      "};"
      "int main(void) {"
      "  union A a;"
      "  a.a = 42;"
      "  return a.b;"
      "}",
   .ret_val = 42,
},
{
   .name = "addr-of union-member",
   .compiles = true,
   .source =
      "union A {"
      "  int a;"
      "  int b;"
      "};"
      "int main(void) {"
      "  union A a;"
      "  return &a.a == &a.b;"
      "}",
   .ret_val = 1,
},
{
   .name = "sizeof(union)",
   .compiles = true,
   .source =
      "union A {"
      "  int a;"
      "  byte b;"
      "};"
      "int main(void) {"
      "  union A a;"
      "  return sizeof(a) == sizeof(int);"
      "}",
   .ret_val = 1,
},
{
   .name = "struct/union mismatch",
   .compiles = false,
   .source =
      "struct A {};"
      "int main(void) {"
      "  union A a;"
      "}",
},
{
   .name = "union/struct mismatch",
   .compiles = false,
   .source =
      "union A {};"
      "int main(void) {"
      "  struct A a;"
      "}",
},
{
   .name = "multi-delaration in struct",
   .compiles = true,
   .source =
      "struct A { byte a, b; };"
      "int main(void) { return sizeof(struct A); }",
   .ret_val = 2,
},
{
   .name = "indirect function call",
   .compiles = true,
   .source =
      "int add(int a, int b) { return a + b; }"
      "int main(void) {"
      "  auto f = &add;"
      "  return f(3, 2);"
      "}",
   .ret_val = 5,
},
{
   .name = "inner struct",
   .compiles = true,
   .source =
      "struct A {"
      "  int a;"
      "  struct B {"
      "     int b;"
      "  };"
      "};"
      "int main(void) {"
      "  struct B b;"
      "  b.b = 42;"
      "  return b.b;"
      "}",
   .ret_val = 42,
},
{
   .name = "nested unnamed struct",
   .compiles = true,
   .source =
      "struct A {"
      "  struct {"
      "     int a;"
      "  };"
      "};"
      "int main(void) {"
      "  struct A a;"
      "  a.a = 42;"
      "  return a.a;"
      "}",
   .ret_val = 42,
},
{
   .name = "redefinition of variable in struct",
   .compiles = false,
   .source =
      "struct A {"
      "  int a;"
      "  int a;"
      "};"
      "int main(void) {}",
},
{
   .name = "function pointer",
   .compiles = true,
   .source =
      "int add(int a, int b) { return a + b; }"
      "int main(void) {"
      "  int(int, int)* f= &add;"
      "}",
},
{
   .name = "typeof(int)",
   .compiles = true,
   .source =
      "int printf(const char*, ...);"
      "int main(void) {"
      "  printf(\"typeof(int)=%s\", typeof(int));"
      "}",
   .output = "typeof(int)=int",
},
{
   .name = "typeof(func-ptr)",
   .compiles = true,
   .source =
      "int puts(const char*);"
      "int add(int, int);"
      "int main(void) {"
      "  puts(typeof(&add));"
      "}",
   .output = "int(int, int)* const\n",
},
{
   .name = "stdint",
   .compiles = true,
   .source =
      "typedef __builtin_uint8_t uint8_t;"
      "typedef __builtin_uint16_t uint16_t;"
      "typedef __builtin_uint32_t uint32_t;"
      "int main(void) { return sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint32_t); }",
   .ret_val = 7,
},
{
   .name = "simple switch/case",
   .compiles = true,
   .source =
      "int main(void) {"
      "  int a = 99;"
      "  switch (a) {"
      "  case 99:"
      "     return 98;"
      "  default:"
      "     return 42;"
      "  }"
      "}",
   .ret_val = 98,
},
{
   .name = "declaration & definition of function",
   .compiles = true,
   .source =
      "int add(int, int);"
      "int main(void) {"
      "  return add(3, 2);"
      "}"
      "int add(int a, int b) {"
      "  return a + b;"
      "}",
   .ret_val = 5,
},
{
   .name = "remove \\\\n (pre-processor)",
   .compiles = true,
   .source =
      "int printf(const char*, ...);"
      "int main(void) {"
      "  printf(\"Hello \\\n"
      "World\\n\");"
      "}",
   .output = "Hello World\n",
   .ret_val = 0,
},
{
   .name = "predefined macros",
   .compiles = true,
   .source =
      "int main(void) {"
      "  return __bcc__;"
      "}",
   .ret_val = 1,
},
{
   .name = "volatile variable",
   .compiles = true,
   .source =
      "int main(void) {"
      "  volatile int x = 42;"
      "  x = 49;"
      "  return x;"
      "}",
   .ret_val = 49,
},
{
   .name = "volatile pointer",
   .compiles = true,
   .source =
      "int main(void) {"
      "  volatile int* volatile p;"
      "  return 0;"
      "}",
   .ret_val = 0,
},
{
   .name = "copy struct",
   .compiles = true,
   .source =
      "struct S {"
      "  int a;"
      "};"
      "int main(void) {"
      "  struct S s;"
      "  s.a = 42;"
      "  struct S t = s;"
      "  return t.a;"
      "}",
   .ret_val = 42,
},
{
   .name = "copy struct 2",
   .compiles = true,
   .source =
      "struct S {"
      "  int a;"
      "};"
      "int main(void) {"
      "  struct S s;"
      "  s.a = 42;"
      "  struct S t;"
      "  t = s;"
      "  return t.a;"
      "}",
   .ret_val = 42,
},
{
   .name = "return struct",
   .compiles = true,
   .source =
      "struct S {"
      "  int a;"
      "};"
      "struct S get_S(void) {"
      "  struct S s;"
      "  s.a = 42;"
      "  return s;"
      "int main(void) {"
      "  struct S s;"
      "  s = get_S();"
      "  return s;"
      "}",
   .ret_val = 42,
},
{
   .name = "pass struct",
   .compiles = true,
   .source =
      "struct S {"
      "  int a;"
      "};"
      "int f(struct S s) { return s.a; }"
      "int main(void) {"
      "  struct S s;"
      "  s.a = 42;"
      "  return f(s);"
      "}",
   .ret_val = 42,
},
{
   .name = "preprocessor #",
   .compiles = true,
   .source =
      "#define str(x) #x\n"
      "int puts(const char*);"
      "int main(void) {"
      "  puts(str(42));"
      "}",
   .output = "42\n",
},
{
   .name = "nested macro functions",
   .compiles = false,
   .source =
      "#define f(x) x\n"
      "#define f(x) f(x)\n"
      "int main(void) { return f(42); }",
},
{
   .name = "#include <stdint.h>",
   .compiles = true,
   .source =
      "#include <stdint.h>\n"
      "int main(void) {"
      "  return sizeof(uint32_t);"
      "}",
   .ret_val = 4,
},
{
   .name = "restrict",
   .compiles = true,
   .source =
      "int inc(int* restrict a) { ++*a; }"
      "int main(void) {"
      "  int x = 41;"
      "  inc(&x);"
      "  return x;"
      "}",
   .ret_val = 42,
},
{
   .name = "invalid use of restrict",
   .compiles = false,
   .source =
      "int f(restrict int x);"
      "int main() {}"
},
{
   .name = "invalid use of restrict 2",
   .compiles = false,
   .source =
      "int main() { restrict int* x; }"
},
{
   .name = "_Bool",
   .compiles = true,
   .source =
      "_Bool toBool(int x) { return x; }"
      "int toInt(_Bool x) { return x; }"
      "int main(void) {"
      "  _Bool b = toBool(42);"
      "  int i = toInt(b);"
      "  return b + i;"
      "}",
   .ret_val = 2,
},
{
   .name = "noreturn",
   .compiles = true,
   .source =
      "#include <stdnoreturn.h>\n"
      "noreturn void exit(int s);"
      "int main(void) {"
      "  exit(42);"
      "}",
   .ret_val = 42,
},
{
   .name = "true static assert",
   .compiles = true,
   .source =
      "int main(void) {"
      "  _Static_assert(42 == 42);"
      "  return 0;"
      "}",
},
{
   .name = "false static assert",
   .compiles = false,
   .source =
      "int main(void) {"
      "  _Static_assert(42 != 42);"
      "  return 0;"
      "}",
},
{
   .name = "inline",
   .compiles = true,
   .source =
      "inline int add(int x, int y) {"
      "  return x + y;"
      "}"
      "int main(void) {"
      "  return add(3, 2);"
      "}",
   .ret_val = 5,
},
{
   .name = "extern inline",
   .compiles = true,
   .source =
      "inline int add(int x, int y) {"
      "  return x + y;"
      "}"
      "extern inline int add(int, int);"
      "int main(void) {"
      "  return add(3, 2);"
      "}",
   .ret_val = 5,
},
{
   .name = "extern static",
   .compiles = false,
   .source =
      "static int add(int, int);"
      "extern int add(int a, int b) {"
      "  return a + b;"
      "}"
      "int main() {}",
},
{
   .name = "local array initialization",
   .compiles = true,
   .source =
      "int main(void) {"
      "  int a[] = { 1, 2, 3, 4, 5 };"
      "  return (sizeof(a) / sizeof(int)) + a[0] + a[1] + a[2] + a[3] + a[4];"
      "}",
   .ret_val = 20,
},
{
   .name = "local array initialization (char[])",
   .compiles = true,
   .source =
      "int puts(const char*);"
      "int main(void) {"
      "  char str[] = { 'H', 'e', 'l', 'l', 'o', 0 };"
      "  puts(str);"
      "}",
   .output = "Hello\n",
},
{
   .name = "global initialized integer array",
   .compiles = true,
   .source =
      "int printf(const char*, ...);"
      "const int a[] = { 1, 2, 3, 4, 5 };"
      "int main(void) {"
      "  for (unsigned i = 0; i < arraylen(a); ++i)"
      "     printf(\"%d\", a[i]);"
      "  return 0;"
      "}",
   .output = "12345",
},
{
   .name = "global char array from string literal",
   .compiles = true,
   .source =
      "int puts(const char*);"
      "char str[] = \"Hello World\";"
      "int main(void) {"
      "  puts(str);"
      "  return 0;"
      "}",
   .output = "Hello World\n",
},
{
   .name = "local char array from string literal",
   .compiles = true,
   .source =
      "int puts(const char*);"
      "int main(void) {"
      "  char str[] = \"Hello World\";"
      "  puts(str);"
      "  return 0;"
      "}",
   .output = "Hello World\n",
},
{
   .name = "unsized array as parameter",
   .compiles = true,
   .source =
      "int main(int argc, char* argv[]) {"
      "  return 0;"
      "}",
},
