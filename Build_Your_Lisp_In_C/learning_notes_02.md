# Build Your Own Lisp

Learn C and build your own programming language in 1000 lines of code!

Link: [https://www.buildyourownlisp.com/](https://www.buildyourownlisp.com/)

Link: [https://www.buildyourownlisp.com/contents](https://www.buildyourownlisp.com/contents)

Source code: [https://github.com/orangeduck/BuildYourOwnLisp/tree/master/src](https://github.com/orangeduck/BuildYourOwnLisp/tree/master/src)



# Error Handling • Chapter 8



```c
lval eval(mpc_ast_t* t) {

  if (strstr(t->tag, "number")) {
    /* Check if there is some error in conversion */
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
  }

  char* op = t->children[1]->contents;
  lval x = eval(t->children[2]);

  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }

  return x;
}
```

### **`errno` 的作用**

`errno` 是一个全局变量，用于在 C 标准库函数中表示最近一次函数调用的错误状态。许多标准库函数在执行过程中，如果发生错误，会将 `errno` 设置为特定的值以表明错误类型。

在这段代码中：

```c
errno = 0;
long x = strtol(t->contents, NULL, 10);
return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
```

`errno` 的主要作用是检查 `strtol` 是否发生了数值转换错误。

------

### **`errno` 的使用过程**

1. **初始化 `errno` 为 0**：

   ```c
   errno = 0;
   ```

   - 在调用可能出错的函数之前，将 `errno` 设置为 0。
   - 这样可以确保后续的错误检查是基于当前函数调用的结果，而不是之前的其他操作。

2. **调用 `strtol`**：

   ```c
   long x = strtol(t->contents, NULL, 10);
   ```

   - `strtol` 将字符串 `t->contents` 转换为长整型数字。
   - 如果转换过程中发生错误，例如：
     - 字符串内容超出 `long` 类型的范围。
     - 字符串不包含有效的数字。
   - 则 `strtol` 会设置 `errno` 为特定的错误码。

3. **检查 `errno` 的值**：

   ```c
   return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
   ```

   - `errno != ERANGE` 检查是否发生了范围错误（`ERANGE` 表示 "range error"）。
   - 成功：
     - 如果 `errno` 不是 `ERANGE`，说明数值转换成功，返回一个 `lval_num`。
   - 失败：
     - 如果 `errno` 是 `ERANGE`，说明数值超出范围，返回一个错误值 `lval_err(LERR_BAD_NUM)`。

------

### **`ERANGE` 的含义**

`ERANGE` 是一个宏，定义在 `<errno.h>` 中，表示范围错误。
 对于 `strtol`，`ERANGE` 的典型触发场景是：

1. 转换结果小于 `LONG_MIN` 或大于 `LONG_MAX`。
2. 例如：
   - 字符串 `"9223372036854775808"` 超过 `long` 类型范围（假设 64 位系统）。
   - 字符串 `"-9223372036854775809"` 小于 `long` 的最小值。

------

### **完整示例**

#### 合法输入：

```c
errno = 0;
long x = strtol("123", NULL, 10);
if (errno != ERANGE) {
    printf("Conversion succeeded: %ld\n", x); // 输出: Conversion succeeded: 123
} else {
    printf("Conversion failed: Out of range.\n");
}
```

#### 非法输入：

```c
errno = 0;
long x = strtol("999999999999999999999999", NULL, 10);
if (errno != ERANGE) {
    printf("Conversion succeeded: %ld\n", x);
} else {
    printf("Conversion failed: Out of range.\n"); // 输出: Conversion failed: Out of range.
}
```

------

### **总结**

1. **`errno = 0`**：初始化为 0，确保检查的错误状态是当前函数调用产生的。
2. **`strtol` 设置 `errno`**：如果转换失败（例如数值超出范围），`strtol` 会将 `errno` 设置为 `ERANGE`。
3. `errno != ERANGE`：通过检查 `errno`的值，判断转换是否成功。
   - 如果成功，返回转换后的数值。
   - 如果失败，返回错误值或采取其他错误处理逻辑。



`strtol` 是 C 标准库中的一个函数，用于将字符串转换为长整型（`long`）。它的原型定义在 `<stdlib.h>` 头文件中，函数签名如下：

```c
long int strtol(const char *str, char **endptr, int base);
```

### 参数说明
1. **`const char *str`**  
   这是要转换的字符串。字符串可以包含数字字符（0-9）、字母字符（a-z 或 A-Z，取决于进制），以及可选的符号（`+` 或 `-`）。

2. **`char **endptr`**  
   这是一个指向字符指针的指针。`strtol` 会将转换过程中第一个无法识别的字符的地址存储到 `*endptr` 中。如果不需要这个信息，可以传递 `NULL`。

3. **`int base`**  
   这是转换的进制基数，范围是 2 到 36。  
   - 如果 `base` 是 10，则字符串被解释为十进制数。
   - 如果 `base` 是 16，则字符串被解释为十六进制数（可以包含 `0x` 前缀）。
   - 如果 `base` 是 0，则函数会根据字符串的前缀自动推断进制：
     - 以 `0x` 或 `0X` 开头的字符串被解释为十六进制。
     - 以 `0` 开头的字符串被解释为八进制。
     - 其他情况被解释为十进制。

### 返回值
- 如果转换成功，`strtol` 返回转换后的长整型值。
- 如果字符串无法转换（例如字符串为空或不合法），则返回 0。
- 如果转换结果超出了 `long` 的表示范围，则返回 `LONG_MAX` 或 `LONG_MIN`，并设置 `errno` 为 `ERANGE`。

---

### 示例代码
```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    const char *str = "123";
    char *endptr;
    long x = strtol(str, &endptr, 10);

    if (str == endptr) {
        printf("No valid conversion occurred.\n");
    } else {
        printf("Converted value: %ld\n", x);
    }

    return 0;
}
```

### 代码解释
1. **`str = "123"`**  
   这是要转换的字符串。

2. **`endptr`**  
   用于存储转换结束的位置。如果转换成功，`endptr` 会指向字符串中第一个无法转换的字符。

3. **`base = 10`**  
   表示字符串是十进制数。

4. **`str == endptr`**  
   如果转换没有发生（例如字符串为空或不合法），`endptr` 会指向原始字符串的起始位置。

---

### 关于 `10` 的含义
在你的代码中，`10` 表示字符串 `"123"` 是一个十进制数。`strtol` 会按照十进制规则解析字符串，将其转换为长整型值 `123`。

如果 `base` 是 16，字符串可以包含十六进制字符（如 `"0x1A"`），`strtol` 会将其转换为 `26`。

---

### 注意事项
1. **错误处理**  
   如果字符串无法转换，`strtol` 会返回 0。可以通过检查 `endptr` 是否等于原始字符串来判断是否发生了有效转换。

2. **溢出处理**  
   如果转换结果超出 `long` 的范围，`strtol` 会返回 `LONG_MAX` 或 `LONG_MIN`，并设置 `errno` 为 `ERANGE`。

3. **空白字符**  
   `strtol` 会跳过字符串开头的空白字符（如空格、制表符等）。

---





```c
#include "mpc.h"

#ifdef _WIN32

static char buffer[2048];

char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

void add_history(char* unused) {}

#else
#include <editline/readline.h>
//#include <editline/history.h>
#endif

/* Create Enumeration of Possible Error Types */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* Create Enumeration of Possible lval Types */
enum { LVAL_NUM, LVAL_ERR };

/* Declare New lval Struct */
typedef struct {
  int type;
  long num;
  int err;
} lval;

/* Create a new number type lval */
lval lval_num(long x) {
  lval v;
  v.type = LVAL_NUM;
  v.num = x;
  return v;
}

/* Create a new error type lval */
lval lval_err(int x) {
  lval v;
  v.type = LVAL_ERR;
  v.err = x;
  return v;
}

/* Print an "lval" */
void lval_print(lval v) {
  switch (v.type) {
    /* In the case the type is a number print it */
    /* Then 'break' out of the switch. */
    case LVAL_NUM: printf("%li", v.num); break;
    
    /* In the case the type is an error */
    case LVAL_ERR:
      /* Check what type of error it is and print it */
      if (v.err == LERR_DIV_ZERO) {
        printf("Error: Division By Zero!");
      }
      if (v.err == LERR_BAD_OP)   {
        printf("Error: Invalid Operator!");
      }
      if (v.err == LERR_BAD_NUM)  {
        printf("Error: Invalid Number!");
      }
    break;
  }
}

/* Print an "lval" followed by a newline */
void lval_println(lval v) { lval_print(v); putchar('\n'); }

lval eval_op(lval x, char* op, lval y) {
  
  /* If either value is an error return it */
  if (x.type == LVAL_ERR) { return x; }
  if (y.type == LVAL_ERR) { return y; }
  
  /* Otherwise do maths on the number values */
  if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
  if (strcmp(op, "-") == 0) { return lval_num(x.num - y.num); }
  if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
  if (strcmp(op, "/") == 0) {
    /* If second operand is zero return error */
    return y.num == 0 
      ? lval_err(LERR_DIV_ZERO) 
      : lval_num(x.num / y.num);
  }
  
  return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t) {
  
  if (strstr(t->tag, "number")) {
    /* Check if there is some error in conversion */
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
  }
  
  char* op = t->children[1]->contents;  
  lval x = eval(t->children[2]);
  
  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }
  
  return x;  
}

int main(int argc, char** argv) {
  
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Lispy = mpc_new("lispy");
  
  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                     \
      number   : /-?[0-9]+/ ;                             \
      operator : '+' | '-' | '*' | '/' ;                  \
      expr     : <number> | '(' <operator> <expr>+ ')' ;  \
      lispy    : /^/ <operator> <expr>+ /$/ ;             \
    ",
    Number, Operator, Expr, Lispy);
  
  puts("Lispy Version 0.0.0.0.4");
  puts("Press Ctrl+c to Exit\n");
  
  while (1) {
  
    char* input = readline("lispy> ");
    add_history(input);
    
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      lval result = eval(r.output);
      lval_println(result);
      mpc_ast_delete(r.output);
    } else {    
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    
    free(input);
    
  }
  
  mpc_cleanup(4, Number, Operator, Expr, Lispy);
  
  return 0;
}
```

测试

```c
cc -std=c99 -Wall error_handling.c mpc.c -ledit -o error_handling   
./error_handling 
Lispy Version 0.0.0.0.4
Press Ctrl+c to Exit

lispy> * 3 10
30
lispy> / 40 0
Error: Division By Zero!
lispy> 
```



# S-Expressions • Chapter 9

### 什么是 Lisp 的 S-Expression？

在 Lisp 编程语言中，**S-Expression（Symbolic Expression，符号表达式）**是 Lisp 代码和数据的核心表示形式。它是一种递归的数据结构，用于表达程序的语法和数据的结构。S-Expression 简洁且通用，是 Lisp 强大和灵活的根源之一。

------

#### 1. S-Expression 的基本概念

S-Expression 是由两种基本元素组成的：

1. **原子（Atom）**：数字、符号或字符串等不可再分的元素。例如：`42`、`hello`、`"string"`。
2. **列表（List）**：由括号括起来的一组元素，这些元素可以是原子，也可以是嵌套的列表。例如：`(1 2 3)`、`(+ 1 2)`、`(define (square x) (* x x))`。

一个典型的 S-Expression 可以是原子，也可以是列表，列表中的元素还可以是其他 S-Expression，这种递归的结构使得 S-Expression 能够表示任意复杂的程序逻辑。

------

#### 2. S-Expression 的形式和作用

**形式**
 S-Expression 的语法非常简单，遵循以下规则：

- 一个原子是一个单独的符号或值，如 `x` 或 `42`。
- 一个列表由括号包围，元素之间用空格分隔，如 `(1 2 3)`。
- 列表中的元素可以是原子或其他列表，例如：`(1 (2 3) 4)`。

**作用**

- **代码即数据**：Lisp 的最大特点之一是“代码即数据（Code as Data）”。这意味着 Lisp 程序本身就是用 S-Expression 表示的，同时 S-Expression 也是数据结构。这种统一性允许程序动态修改自己的代码（元编程）。
- **统一解析**：由于 S-Expression 的简单语法，解析器和解释器可以轻松解析并执行代码。
- **递归表示**：S-Expression 的递归结构非常适合用递归方式处理程序逻辑，例如计算表达式、构建语法树等。

------

#### 3. S-Expression 的例子

以下是一些常见的 S-Expression 示例：

- **简单列表**：`(1 2 3 4)`
   表示一个包含 4 个元素的列表。

- **嵌套列表**：`(1 (2 3) 4)`
   表示一个列表，其中第二个元素本身是一个子列表。

- **函数调用**：`(+ 1 2 3)`
   表示调用加法函数 `+`，计算 1 + 2 + 3 的值。

- **定义函数**：

  ```lisp
  (define (square x) (* x x))
  ```

  表示定义一个名为 `square` 的函数，它接受一个参数 `x`，返回 `x * x`。

------

#### 4. 在 C 语言中实现 S-Expression

为了在 C 语言中实现 S-Expression，可以设计一个递归的数据结构来表示原子和列表。例如：

```c
typedef struct lval {
    int type;              // 表示数据类型：数字、符号或列表
    long num;              // 数字值
    char* sym;             // 符号值
    struct lval** cell;    // 子列表
    int count;             // 子列表中的元素数量
} lval;
```

- **原子**：通过 `type` 标识数字或符号。
- **列表**：通过 `cell` 存储子元素，递归表示列表中的每个元素。

------

#### 5. S-Expression 的意义

- **灵活性**：S-Expression 能以最简单的方式表达复杂的程序逻辑。
- **统一性**：代码和数据共享同一种表示形式，使得 Lisp 成为研究元编程和 DSL（领域特定语言）的理想工具。
- **简洁性**：解析器和解释器可以以简单的实现支持 S-Expression，降低语言实现的复杂性。

------

#### 6. 总结

S-Expression 是 Lisp 的核心特性之一，基于递归的结构，能够简单而优雅地表示代码和数据。在实现一个 Lisp 解释器时，理解和构建 S-Expression 是关键的一步。无论是在学习 Lisp 编程，还是设计自己的语言，实现 S-Expression 都是一个不可忽视的基础。

通过学习 S-Expression，不仅能够理解 Lisp 的设计理念，还能掌握递归思维和语言设计的核心思想。





具体代码链接：[https://github.com/orangeduck/BuildYourOwnLisp/blob/master/src/s_expressions.c](https://github.com/orangeduck/BuildYourOwnLisp/blob/master/src/s_expressions.c)

```c
lval* lval_read_num(mpc_ast_t* t) {
  errno = 0;
  long x = strtol(t->contents, NULL, 10);
  return errno != ERANGE ?
    lval_num(x) : lval_err("invalid number");
}
```



```c
lval* lval_read(mpc_ast_t* t) {

  /* If Symbol or Number return conversion to that type */
  if (strstr(t->tag, "number")) { return lval_read_num(t); }
  if (strstr(t->tag, "symbol")) { return lval_sym(t->contents); }

  /* If root (>) or sexpr then create empty list */
  lval* x = NULL;
  if (strcmp(t->tag, ">") == 0) { x = lval_sexpr(); }
  if (strstr(t->tag, "sexpr"))  { x = lval_sexpr(); }

  /* Fill this list with any valid expression contained within */
  for (int i = 0; i < t->children_num; i++) {
    if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
    if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
    if (strcmp(t->children[i]->tag,  "regex") == 0) { continue; }
    x = lval_add(x, lval_read(t->children[i]));
  }

  return x;
}
```





```c
lval* lval_add(lval* v, lval* x) {
  v->count++;
  v->cell = realloc(v->cell, sizeof(lval*) * v->count);
  v->cell[v->count-1] = x;
  return v;
}
```



代码中 `v->cell[v->count-1] = x;` 的关键在于 **数组索引与元素个数的关系**。我们逐步分析代码为什么要使用 `v->count - 1`。

------

### **数组与索引的基本关系**

在 C 中，数组的索引从 **0 开始**，这意味着：

- 如果数组有 1 个元素，则该元素的索引是 `0`；
- 如果数组有 2 个元素，索引是 `0` 和 `1`；
- 如果数组有 `n` 个元素，索引是 `0` 到 `n-1`。

因此，访问数组最后一个元素的索引是 `count - 1`。

------

### **代码功能分析**

#### **代码片段**

```c
lval* lval_add(lval* v, lval* x) {
  v->count++;  // 增加表达式列表中的元素计数
  v->cell = realloc(v->cell, sizeof(lval*) * v->count);  // 重新分配存储空间
  v->cell[v->count-1] = x;  // 将新元素添加到最后一个位置
  return v;
}
```

#### **代码逻辑解析**

1. **`v->count++`**：

   - `v->count` 表示列表中的元素个数。
   - 调用 `lval_add` 时，我们新增了一个元素，因此需要将计数加 1。

2. **`v->cell = realloc(...)`**：

   - 使用 `realloc` 为 `v->cell` 分配更多的内存空间，以便存储新增的元素。
   - 新分配的数组大小是 `sizeof(lval*) * v->count`，确保能容纳所有元素。

3. **`v->cell[v->count-1] = x;`**：

   - 为什么是 `v->count - 1`

     ：

     - 新增一个元素后，数组的最后一个位置正好是新增的那一项，而它的索引是 `count - 1`。
     - 如果直接用 `v->count`，会超出数组的有效范围，导致非法内存访问。

------

### **例子说明**

假设 `v->count` 初始值为 2，`v->cell` 中已经有两个元素，分别存储在索引 `0` 和 `1`。

#### **调用 `lval_add`**

- **执行 `v->count++`**：
  - `v->count` 从 2 增加到 3，表示数组现在需要存储 3 个元素。
- **执行 `realloc`**：
  - `v->cell` 被重新分配空间，大小足够存储 3 个元素。
- **执行 `v->cell[v->count-1] = x`**：
  - 现在 `v->count = 3`，所以新增元素的索引是 `v->count - 1 = 2`。
  - 将新元素 `x` 存储在索引 `2` 的位置。

------

### **如果没有 `-1` 会发生什么？**

如果直接写成 `v->cell[v->count] = x;`，问题是：

1. 数组的索引是从 0 到 `count - 1`，`v->count` 超出有效范围。
2. 访问 `v->cell[v->count]` 会导致 **非法内存访问**，程序崩溃。

------

### **总结**

- **`v->count` 表示数组的元素总数**。
- **`v->count - 1` 是新增元素的位置索引**，因为数组索引从 0 开始。
- 使用 `v->cell[v->count-1] = x;` 可以确保新元素正确存储在最后一个位置。



解释代码：

```c
lval* lval_pop(lval* v, int i) {
  /* Find the item at "i" */
  lval* x = v->cell[i];

  /* Shift memory after the item at "i" over the top */
  memmove(&v->cell[i], &v->cell[i+1],
    sizeof(lval*) * (v->count-i-1));

  /* Decrease the count of items in the list */
  v->count--;

  /* Reallocate the memory used */
  v->cell = realloc(v->cell, sizeof(lval*) * v->count);
  return x;
}
```

### **1. `memmove` 的作用**

这段代码的核心是 `memmove` 的操作，它用于将内存块从一个位置移动到另一个位置。

#### **代码片段**

```c
memmove(&v->cell[i], &v->cell[i+1], sizeof(lval*) * (v->count - i - 1));
```

#### **解释**

- **`&v->cell[i]`**：目标地址，从索引 `i` 开始的位置。
- **`&v->cell[i+1]`**：源地址，从索引 `i+1` 开始的位置。
- **`sizeof(lval*) * (v->count - i - 1)`**：要移动的内存大小，表示从 `i+1` 到 `v->count-1` 的所有元素。

#### **具体作用**

1. 将从索引 `i+1` 开始的所有元素，向前移动一个位置，覆盖索引 `i` 处的内容。
2. 目的是移除第 `i` 个元素，同时保持数组的连续性。

------

### **示例**

假设我们有一个 `lval` 的数组 `v->cell`，如下：

```c
v->cell = [A, B, C, D];
v->count = 4;
```

#### **执行 `lval_pop(v, 1)`**

即移除索引 `1` 的元素（`B`），具体过程如下：

1. **找到 `B`**：

   ```c
   lval* x = v->cell[1];  // 提取 B
   ```

   此时，`x = B`。

2. **调用 `memmove`**：

   ```c
   memmove(&v->cell[1], &v->cell[2], sizeof(lval*) * (4 - 1 - 1));
   ```

   - **`&v->cell[1]`** 是目标地址（索引 1）。
   - **`&v->cell[2]`** 是源地址（索引 2）。
   - **`sizeof(lval\*) \* (4 - 1 - 1) = sizeof(lval\*) \* 2`**，表示移动两个元素。

   移动后数组变为：

   ```c
   v->cell = [A, C, D, D];  // 索引 2 的元素覆盖了索引 1 的位置
   ```

3. **更新 `v->count`**：

   ```c
   v->count--;  // 数组长度减 1
   ```

   此时：

   ```c
   v->cell = [A, C, D];  // 长度变为 3
   ```

4. **重新分配内存**：

   ```c
   v->cell = realloc(v->cell, sizeof(lval*) * v->count);
   ```

   - 新的内存大小是 `sizeof(lval*) * 3`，释放了不再需要的空间。

------

### **2. `realloc` 的作用**

`realloc` 用于重新分配内存，调整数组的大小。

#### **代码片段**

```c
v->cell = realloc(v->cell, sizeof(lval*) * v->count);
```

#### **解释**

- **`v->cell`**：原始数组的指针。
- **`sizeof(lval\*)`**：单个指针的大小。
- **`v->count`**：当前数组的元素数量。

#### **作用**

1. 根据当前的元素数量 `v->count`，调整数组的内存大小。
2. 如果数组变小，则多余的空间会被释放；如果数组变大，则分配更多的空间。
3. 返回新的指针，如果内存重新分配成功，指向调整后的数组。

#### **示例**

假设当前数组 `v->cell`：

```c
v->cell = [A, C, D, NULL];  // 长度为 4
```

1. 调用 `realloc`：

   ```c
   v->cell = realloc(v->cell, sizeof(lval*) * 3);
   ```

2. 新的数组内存分配：

   ```c
   v->cell = [A, C, D];  // 长度调整为 3，释放了不需要的部分
   ```

------

### **总结**

1. **`memmove` 的功能**：
   - 将从索引 `i+1` 开始的元素向前移动，覆盖索引 `i` 的位置，达到移除元素的目的。
   - 示例：从 `[A, B, C, D]` 变为 `[A, C, D]`。
2. **`realloc` 的功能**：
   - 重新分配内存，使数组的大小与元素数量一致，避免浪费空间。
   - 示例：从长度 4 调整为长度 3。

两者结合确保移除元素后，数组既保持逻辑连续性，又释放了多余的内存。



测试代码：

```c
cc -std=c99 -Wall s_expressions.c mpc.c -ledit -o s_expressions   
./s_expressions 
Lispy Version 0.0.0.0.5
Press Ctrl+c to Exit

lispy> + 1 (* 7 5) 3 
39
lispy> (-100)
-100
lispy> 
()
lispy> /
/
lispy> (/ ())
Error: Cannot operate on non-number!
lispy> 
```



表达式 `+ 1 (* 7 5) 3` 的解析中，**`+` 是一个多元操作符，而不是一个严格的二元操作符**。因此，这里需要解释清楚为什么 `+` 需要三个操作数。

------

### **前缀表达式的特点**

1. **前缀表达式规则**：
   - 操作符后面直接跟操作数，按照从左到右的顺序依次计算。
   - 操作符可以作用于多个操作数（在某些 Lisp 风格的表达式中，操作符往往是**多元的**，而不是固定的二元）。
2. **多元操作符**：
   - 在这个表达式中，`+` 是一个多元操作符，可以接收任意多个操作数并计算其和。
   - 例如：
     - `+ 1 2` → `1 + 2`
     - `+ 1 2 3` → `1 + 2 + 3`

------

### **为什么这里需要三个操作数？**

在表达式 `+ 1 (* 7 5) 3` 中：

1. **`+` 是操作符**：
   - `+` 需要处理其后面的所有操作数，直到遇到整个表达式的结束。
2. **操作数依次是：**
   - 第一个操作数：`1`
   - 第二个操作数：`(* 7 5)`（这是一个子表达式，需要先计算）
   - 第三个操作数：`3`
3. **多元计算逻辑**：
   - 在多元运算中，`+` 会将 `1`、`(* 7 5)` 的结果、`3` 依次累加。

------

### **总结**

- +需要三个操作数的原因是：
  1. **这是一个多元操作符**，而非严格的二元操作符。
  2. 它会作用于其后所有的操作数。
  3. 表达式 `+ 1 (* 7 5) 3` 是合法的前缀表达式，其含义是对 `1`、`(* 7 5)` 和 `3` 求和，计算顺序由嵌套的子表达式优先级决定。

最终，这种逻辑符合 Lisp 风格的前缀表示法规则。

在 Lisp 风格的前缀表达式中，操作符（如 `+`）可以接受任意数量的操作数。操作数的数量不受限制，表达式计算时从左到右依次累加所有操作数，遵循嵌套优先的规则。你可以写出像 `+ 1 2 3 4` 或 `+ 1 2 3 4 5 6` 这样的表达式，计算逻辑完全相同。



# Q-Expressions • Chapter 10



You'll notice that the following chapters will all follow a similar pattern. This pattern is the typical approach used to add new features to a language. It consists of a number of steps that bring a feature from start to finish. These are listed below, and are exactly what we're going to do in this chapter to introduce a new feature called a Q-Expression.

| **Syntax**         | Add new rule to the language grammar for this feature.       |
| ------------------ | ------------------------------------------------------------ |
| **Representation** | Add new data type variation to represent this feature.       |
| **Parsing**        | Add new functions for reading this feature from the *abstract syntax tree*. |
| **Semantics**      | Add new functions for evaluating and manipulating this feature. |

你提到的 **pattern（模式）** 是在基于 C 语言实现一门 Lisp 风格编程语言时，为添加新特性（例如 Q-Expression）而遵循的一个通用开发流程。这个模式是语言设计中的经典步骤，用于逐步将一个新功能从概念落实到实际实现。

以下是这个模式的详细解读：

------

### **1. Syntax（语法）**

**目的**：定义新特性在语言中的表达形式，即 **语法规则**。

- 工作内容：

  - 更新语言的语法（grammar），为新特性添加对应的解析规则。
  - 使用工具（例如 `mpc`）为语法树新增节点类型，让新特性成为语言的一部分。

- 示例： 假设我们要引入 Q-Expression，可以在文法中添加：

  ```c
  qexpr : '{' <expr>* '}' ;
  ```

  - `qexpr` 定义了用 `{}` 包裹的表达式。
  - 这是 Q-Expression 的基础语法形式。

------

### **2. Representation（表示）**

**目的**：为新特性添加 **数据结构**，让它能够被表示和存储。

- **工作内容**：

  - 更新语言的数据结构（如 `lval` 类型），新增一个变体来表示新特性。
  - 确保程序能够识别新特性并将其存储在语法树中。

- **示例**： 在 `lval` 类型中新增一个枚举值和对应的数据字段：

  ```c
  typedef struct lval {
    int type;
    union {
      // 其他数据类型（数字、错误、符号等）
      struct { int count; struct lval** cell; } qexpr; // Q-Expression
    };
  };
  
  // 新增枚举值
  #define LVAL_QEXPR 4
  ```

------

### **3. Parsing（解析）**

**目的**：定义从抽象语法树（AST）中 **解析新特性** 的逻辑。

- **工作内容**：

  - 编写解析函数，从 AST 中提取新特性的内容。
  - 确保语法树中的新节点能被正确识别和处理。

- **示例**： 为 Q-Expression 编写解析函数：

  ```c
  lval* lval_read_qexpr(mpc_ast_t* t) {
    lval* x = lval_qexpr(); // 创建一个空的 Q-Expression
    for (int i = 0; i < t->children_num; i++) {
      if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
      if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
      x = lval_add(x, lval_read(t->children[i])); // 递归解析子节点
    }
    return x;
  }
  ```

------

### **4. Semantics（语义）**

**目的**：为新特性添加 **操作和行为逻辑**，使其有意义并能够运行。

- **工作内容**：

  - 实现新特性的语义，定义其行为和如何被计算。
  - 更新解释器中的求值函数（如 `eval`），让新特性能被运行。

- **示例**： 实现 Q-Expression 的求值逻辑：

  ```c
  lval* lval_eval_qexpr(lval* v) {
    for (int i = 0; i < v->count; i++) {
      v->cell[i] = lval_eval(v->cell[i]); // 递归求值每个子表达式
    }
    return v; // 返回处理后的 Q-Expression
  }
  ```

------

### **总结：Pattern 的核心逻辑**

1. **Syntax（语法）**：确定新特性的语法规则，告诉解释器它“长什么样”。
2. **Representation（表示）**：更新语言的数据结构，确保新特性能被存储和识别。
3. **Parsing（解析）**：从语法树中提取新特性，并存储为内部数据结构。
4. **Semantics（语义）**：定义新特性的行为，确保它能被正确计算和运行。

通过这个模式，你可以系统化地设计和实现任何语言特性。这种模式将 **语言设计的理论部分**（语法和语义）与 **编程实现的工程部分**（数据结构和解析逻辑）有机结合，是语言开发的核心方法。



### **抽象语法树（AST）的介绍**

抽象语法树（Abstract Syntax Tree，简称 AST）是编译原理中一个非常重要的概念。它是程序源代码的一种抽象表示，用树状结构展示代码的语法结构。每个节点代表源代码中的一个语法元素，例如变量、操作符、表达式等。

------

### **一、什么是抽象语法树**

抽象语法树是对源代码语法的一种抽象表示。它通过去除不必要的细节（如括号、分号等），专注于表达程序的逻辑结构。相比于源代码的文本表示，AST 更适合用于编译器、解释器和其他程序分析工具对代码进行处理。

#### **特点**：

1. **抽象性**：AST 不直接对应源代码的文本表示，而是以逻辑结构为主。
2. **层次性**：AST 是一种分层结构，根节点代表整个程序，子节点依次代表语法成分。
3. **易操作性**：AST 便于对代码进行分析、优化、转换等操作。

------

### **二、抽象语法树的构造过程**

构建 AST 的过程通常分为以下几步：

1. **词法分析（Lexical Analysis）**：

   - 将源代码分解为一系列标记（Token）。

   - 例如，x = 3 + 5 会被分解为：

     ```c
     标识符（x），赋值符号（=），数字（3），加号（+），数字（5）
     ```

2. **语法分析（Parsing）**：

   - 根据语法规则将标记序列组织成语法结构。
   - 这一步会生成初步的语法树（Concrete Syntax Tree, CST）。

3. **抽象化**：

   - 将语法树中的冗余节点去除，保留关键语法信息，形成抽象语法树（AST）。
   - 例如，括号、分号等仅在语法规则中有作用，但不影响逻辑的部分会被省略。

------

### **三、抽象语法树的结构**

以下是 AST 的基本结构：

1. **节点（Node）**：
   - 每个节点表示一个语法元素，例如操作符、变量、常量等。
   - 节点通常包含以下信息：
     - 类型：如运算符、数字、变量等。
     - 值：节点对应的具体值（如数字 5、变量名 `x`）。
2. **边（Edge）**：
   - 节点之间的边表示语法元素的从属关系。
   - 例如，在表达式 `3 + 5` 中，加号是根节点，数字 3 和 5 是其子节点。

------

### **四、抽象语法树的示例**

以表达式 `x = 3 + 5` 为例：

#### **源代码**：

```c
x = 3 + 5;
```

#### **对应的 AST**：

```c
Assignment (=)
├── Identifier (x)
└── Add (+)
    ├── Number (3)
    └── Number (5)
```

解释：

1. 根节点是赋值操作符 `=`，表示赋值表达式。
2. 左子节点是变量 `x`，右子节点是加法表达式。
3. 加法表达式节点 `+` 有两个子节点：数字 `3` 和 `5`。

------

### **五、抽象语法树的作用**

1. **代码解析**：
   - 编译器可以通过 AST 了解代码的结构，进行语法检查。
2. **代码优化**：
   - AST 可以帮助编译器识别重复计算、简化表达式，从而优化代码性能。
3. **代码生成**：
   - 编译器根据 AST 生成目标代码（如机器代码或字节码）。
4. **静态分析**：
   - 工具可以通过 AST 进行类型检查、依赖分析等。
5. **代码转换**：
   - AST 便于将代码从一种语言转换为另一种语言，例如将 JavaScript 转换为 WebAssembly。

------

### **六、与具体语法树（CST）的区别**

| 特点       | 抽象语法树（AST）        | 具体语法树（CST）      |
| ---------- | ------------------------ | ---------------------- |
| 关注点     | 表达代码的逻辑结构       | 表达代码的具体语法结构 |
| 包含的内容 | 省略括号、分号等细节     | 包含所有语法细节       |
| 应用场景   | 编译器、优化器、分析工具 | 语法解析器             |

------

### **七、如何操作抽象语法树**

操作 AST 通常包括遍历、修改和生成代码等。以下是一个简单的伪代码示例：

#### **遍历 AST**

```c
void traverse_ast(Node* node) {
    if (node == NULL) return;

    // 访问当前节点
    process(node);

    // 递归访问子节点
    for (int i = 0; i < node->child_count; i++) {
        traverse_ast(node->children[i]);
    }
}
```

------

### **八、总结**

抽象语法树是编程语言实现中不可或缺的工具。它通过清晰的层次结构，将代码的逻辑结构以树的形式表示，便于进行解析、优化和代码生成。无论是编译器的设计，还是代码分析工具的开发，AST 都扮演着重要角色。

#### **关键词**：

- 抽象语法树（AST）
- 语法分析
- 编译器
- 代码优化

通过学习 AST 的原理和应用，你可以更好地理解编译器的内部工作原理，为语言设计和代码分析奠定坚实的基础。

### **Q-Expression 的概念**

Q-Expression（Quoted Expression，**引号表达式**）是 Lisp 中一种特殊的表达式类型。它的核心特点是 **“不被求值”**。在标准的 Lisp 中，表达式（如 S-Expression，Symbolic Expression）通常会在解释器中被求值计算，而 Q-Expression 则直接保留其原始结构。

#### **Q-Expression 的特点**：

1. 不被自动求值：
   - S-Expression 会根据 Lisp 的语义规则被计算，例如执行加法、函数调用等。
   - Q-Expression 则直接保留原始结构，不进行任何计算。
2. 可用于存储和操作：
   - 因为它不被求值，所以可以用于保存数据结构或代码片段。
   - 你可以手动操作或延迟计算它的内容。

#### **Lisp 中 Q-Expression 的表示**：

Q-Expression 通常用 `{}` 包裹内容来表示：

- `{1 2 3}` 表示一个包含 `1`、`2`、`3` 的 Q-Expression。
- `{+ 1 2}` 是一个 Q-Expression，它表示一个加法表达式的原始结构，而不是立即求值为 `3`。

------

### **Q-Expression 与 S-Expression 的区别**

| **特性**     | **S-Expression**                                    | **Q-Expression**                   |
| ------------ | --------------------------------------------------- | ---------------------------------- |
| **求值行为** | 遇到时会被 Lisp 语义规则自动求值                    | 不被求值，直接保留原始结构         |
| **表示法**   | 用 `()` 表示，例如 `(1 2 3)`                        | 用 `{}` 表示，例如 `{1 2 3}`       |
| **用途**     | 执行代码（如加法、函数调用等）                      | 存储代码或数据，延迟计算           |
| **计算结果** | `(1 2 3)` 通常是非法的，但 `(+ 1 2)` 会被求值为 `3` | `{+ 1 2}` 保留为 `{+ 1 2}`，不求值 |

------

### **Q-Expression 的用途**

1. **存储代码块**：
   - Q-Expression 可以用来表示一段代码而不是立即运行它。
   - 例如，`{+ 1 2}` 表示加法操作，而不是计算出 `3`。
2. **延迟求值**：
   - 可以在需要时再手动求值。
   - 例如，通过函数处理 Q-Expression，再调用求值机制。
3. **数据结构**：
   - Q-Expression 可以作为列表或集合，用来存储其他数据。
   - 例如，`{1 2 3}` 是一个 Q-Expression 列表，表示一个简单的容器。
4. **元编程**：
   - 因为 Q-Expression 保留了代码的原始结构，可以通过 Lisp 函数动态修改或生成新的代码。

------

### **示例代码**

#### **S-Expression 的行为**

```lisp
(+ 1 2)  ; 求值后结果是 3
```

#### **Q-Expression 的行为**

```lisp
{+ 1 2}  ; 不求值，保留原始结构为 {+ 1 2}
```

#### **延迟求值**

假设实现了一个手动求值函数 `eval`：

```lisp
eval {+ 1 2}  ; 求值后结果是 3
```

#### **组合与操作**

Q-Expression 可以与 S-Expression 结合使用：

```lisp
(join {+ 1} {2 3})  ; 结果是 {+ 1 2 3}
eval (head {+ 1 2 3})  ; 求值后结果是 3
```

------

### **总结：Q-Expression 的意义**

1. **区别**：
   - Q-Expression 是一种 **静态表示**，保留了表达式的原始形式，而不是立即求值。
   - S-Expression 则是一种 **动态表示**，表示要被立即计算的表达式。
2. **功能**：
   - Q-Expression 提供了延迟求值的能力，可以用作数据存储、代码表示和元编程。
   - 它扩展了标准 Lisp 的功能，使得表达式既能表示操作，又能存储原始结构。

通过引入 Q-Expression，Lisp 风格的语言可以在代码表示与计算之间建立更灵活的机制，为程序的表达和操作提供更大的可能性。

### **什么是宏（Macro）**

宏（Macro）是 Lisp 语言中一种非常强大的功能，它允许程序在运行前动态生成或修改代码。通过宏，Lisp 提供了一种机制，可以对代码进行 **元编程（metaprogramming）**，即代码能够操作其他代码。

#### **特点**

1. **不求值的参数**：宏的参数在宏内部进行操作，而不是被立即求值。
2. **生成代码**：宏会返回新的代码块，这些代码会在调用的位置执行。
3. **灵活性**：宏可以通过程序逻辑动态生成代码，实现复杂的行为。

------

### **在标准 Lisp 中的宏**

Lisp 中的宏是通过 `defmacro` 定义的，行为与函数类似，但最大的区别是 **宏的参数不会被立即求值**。

#### **宏的语法**

```lisp
(defmacro name (args) body)
```

- **`name`**：宏的名称。
- **`args`**：宏的参数列表。
- **`body`**：宏的实现，描述如何操作参数并生成代码。

#### **示例：一个简单的宏**

```lisp
(defmacro square (x)
  `(* ,x ,x))
```

- **定义宏 `square`**：接收参数 `x`，返回表达式 `(* x x)`。
- **反引号**（`）：表示生成代码的模板。
- **逗号**（`,`）：在反引号中表示求值，插入变量的值。

#### **使用宏**

```lisp
(square 3)   ; 展开为 (* 3 3)，然后求值，结果是 9
(square (+ 2 1)) ; 展开为 (* (+ 2 1) (+ 2 1))，结果是 9
```

------

### **特殊宏：`quote`**

`quote` 是 Lisp 中的一个特殊宏，用于阻止代码求值。其简写形式是单引号 `'`。

#### **语法**

```lisp
(quote expression)
```

#### **功能**

1. **阻止求值**：返回表达式的原始形式，而不是其值。
2. **常用于表示数据**：例如列表、符号等。

#### **示例**

```lisp
(quote (+ 1 2)) ; 返回原始表达式：(+ 1 2)
'(+ 1 2)        ; 等价于 (quote (+ 1 2))
```

对比：

```lisp
(+ 1 2) ; 求值后返回 3
'(+ 1 2) ; 返回原始列表：(+ 1 2)
```

------

### **Q-Expression 与 Macro 的区别**

书中提到，Q-Expression 是一种替代 Lisp 宏的新机制。虽然灵感来源于 Lisp 的 `quote` 和宏，但它们的使用方式和语法有一些区别。

| 特性               | Lisp Macro                           | Q-Expression                          |
| ------------------ | ------------------------------------ | ------------------------------------- |
| **定义方式**       | 使用 `defmacro` 定义                 | 用 `{}` 表示的特殊表达式              |
| **是否生成代码**   | 是，宏会返回新的代码块               | Q-Expression 只是存储原始结构         |
| **是否可操作代码** | 是，宏内部可以操作参数并动态生成代码 | 是，Q-Expression 可用作数据并延迟求值 |
| **用途**           | 实现元编程和动态代码生成             | 代码存储、延迟求值、操作代码块        |

------

### **总结：宏与 Q-Expression 的意义**

1. **Lisp 中的宏**：
   - 是阻止求值的机制，可以动态生成代码。
   - 通过像 `quote` 这样的特殊宏，可以灵活操作代码和数据。
2. **Q-Expression 的创新**：
   - Q-Expression 扩展了 `quote` 的功能，将其变为一种更通用的工具。
   - 它不止可以阻止求值，还可以用作代码存储、操作和延迟执行。
3. **对编程的意义**：
   - 宏是 Lisp 强大的元编程工具，能够动态生成高效的代码。
   - Q-Expression 则在这个基础上提供了更清晰、更统一的抽象，使得代码的结构和行为更加直观。







解释下面的 case LVAL_QEXPR和case LVAL_SEXPR的逻辑

```c
void lval_del(lval* v) {

  switch (v->type) {
    case LVAL_NUM: break;
    case LVAL_ERR: free(v->err); break;
    case LVAL_SYM: free(v->sym); break;

    /* If Qexpr or Sexpr then delete all elements inside */
    case LVAL_QEXPR:
    case LVAL_SEXPR:
      for (int i = 0; i < v->count; i++) {
        lval_del(v->cell[i]);
      }
      /* Also free the memory allocated to contain the pointers */
      free(v->cell);
    break;
  }

  free(v);
}
```

是的，`case LVAL_QEXPR:` 和 `case LVAL_SEXPR:` 的执行逻辑是相同的。在这段代码中，它们都对 Q-Expression 和 S-Expression 类型的 `lval` 进行释放操作，具体逻辑如下：

------

### **执行逻辑详解**

#### **代码片段**

```c
case LVAL_QEXPR:
case LVAL_SEXPR:
  for (int i = 0; i < v->count; i++) {
    lval_del(v->cell[i]);
  }
  /* Also free the memory allocated to contain the pointers */
  free(v->cell);
break;
```

#### **核心逻辑**

1. **遍历并释放子元素**：
   - Q-Expression 和 S-Expression 都是容器类型（类似列表），它们的内容保存在 `v->cell` 中，是一个指针数组。
   - 遍历 `v->cell` 中的所有元素，递归调用 `lval_del` 释放每个子元素。
2. **释放存储指针的数组**：
   - 遍历完所有子元素后，`v->cell` 本身是动态分配的内存（例如通过 `malloc` 或 `realloc` 分配）。
   - 调用 `free(v->cell)` 释放这部分内存。
3. **跳出 `case` 块**：
   - 执行完上述操作后，`break` 结束 `switch` 块，避免继续执行其他逻辑。

------

### **`LVAL_QEXPR` 和 `LVAL_SEXPR` 的区别**

在这段代码中，它们的内存释放逻辑是一样的，因为二者都是容器类型，需要：

1. **递归释放子元素**。
2. **释放存储子元素指针的数组**。

#### **在语义上的区别**

尽管释放逻辑相同，但它们的含义不同：

- **`LVAL_QEXPR`（Quoted Expression）**：
  - 表示 **不被求值的表达式**。
  - 用于存储代码或数据，通常表示为 `{}` 包裹的内容，例如 `{+ 1 2}`。
- **`LVAL_SEXPR`（Symbolic Expression）**：
  - 表示 **可求值的表达式**。
  - 用于执行代码逻辑，通常表示为 `()` 包裹的内容，例如 `(+ 1 2)`。

从实现角度看，二者的结构相同，都是通过 `v->cell` 存储子元素，但在求值逻辑上，`Q-Expression` 保留其内容，而 `S-Expression` 会被解释器执行。

------

### **为什么将它们合并到一起？**

1. **结构相同**：
   - `LVAL_QEXPR` 和 `LVAL_SEXPR` 的存储方式相同，都是通过 `v->cell` 动态数组保存子元素，因此释放逻辑完全一致。
2. **代码复用**：
   - 合并处理减少了代码重复，不需要为每种类型单独实现内存释放逻辑。
3. **递归释放**：
   - Q-Expression 和 S-Expression 的子元素可能是嵌套的表达式，递归调用 `lval_del` 确保嵌套的子元素都能被正确释放。

------

### **完整的释放过程（以 Q-Expression 为例）**

假设我们有一个 Q-Expression `{1 {2 3} 4}`，其抽象结构如下：

```c
v (type: LVAL_QEXPR)
├── v->cell[0]: 1 (type: LVAL_NUM)
├── v->cell[1]: {2 3} (type: LVAL_QEXPR)
│   ├── v->cell[0]: 2 (type: LVAL_NUM)
│   └── v->cell[1]: 3 (type: LVAL_NUM)
└── v->cell[2]: 4 (type: LVAL_NUM)
```

#### **释放过程**

1. **主 Q-Expression**：
   - 进入 `case LVAL_QEXPR`。
   - 遍历 `v->cell` 中的每个元素。
2. **释放 `1`**：
   - 递归调用 `lval_del`，进入 `case LVAL_NUM`。
   - 直接返回，不占用动态内存。
3. **释放嵌套 Q-Expression `{2 3}`**：
   - 再次进入 `case LVAL_QEXPR`。
   - 遍历其 `v->cell`，依次释放 `2` 和 `3`。
4. **释放 `4`**：
   - 递归调用 `lval_del`，进入 `case LVAL_NUM`。
5. **释放 `v->cell` 的动态内存**：
   - 主 Q-Expression 和嵌套 Q-Expression 各自释放它们的 `v->cell`。
6. **释放主 Q-Expression 的指针**：
   - 调用 `free(v)`。

------

### **总结**

- **逻辑相同**：`case LVAL_QEXPR` 和 `case LVAL_SEXPR` 的释放逻辑完全相同，因为它们的内部结构一致。
- **语义不同**：`LVAL_QEXPR` 表示不被求值的表达式，而 `LVAL_SEXPR` 表示可被求值的表达式。
- **递归释放**：两者都需要递归释放子元素，确保嵌套结构的所有内存都能被正确释放。
- **内存管理**：释放逻辑确保程序不会发生内存泄漏，这是实现解释器时非常重要的一部分。



### **C语言中的宏（Macro）**

宏（Macro）是C语言预处理器（preprocessor）的一部分，用于在编译之前对代码进行文本替换。宏通过 `#define` 指令定义，可以实现代码的复用、简化复杂表达式以及动态生成代码的功能。宏在C语言中非常灵活，既可以定义简单的符号常量，也可以定义复杂的带参数的代码块。

------

### **宏的特点**

1. 在编译前处理：
   - 宏在预处理阶段展开，不占用运行时资源。
2. 文本替换：
   - 宏实际上是对代码片段的简单文本替换。
3. 无类型检查：
   - 宏没有类型检查，因此更灵活但也更容易出错。
4. 高效：
   - 宏替换后直接编译成目标代码，没有函数调用的开销。

------

### **宏的语法**

#### **1. 定义简单宏**

简单宏将一个名字定义为固定的值或文本。

```c
#define PI 3.14159
#define MESSAGE "Hello, World!"
```

- 使用：

  ```c
  printf("PI: %f\n", PI);           // 输出: PI: 3.14159
  printf("Message: %s\n", MESSAGE); // 输出: Message: Hello, World!
  ```

#### **2. 带参数的宏**

带参数的宏类似于函数，可以接受参数并返回经过替换的代码片段。

```c
#define SQUARE(x) ((x) * (x))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
```

- 使用：

  ```c
  printf("Square of 4: %d\n", SQUARE(4));   // 输出: Square of 4: 16
  printf("Max of 4, 7: %d\n", MAX(4, 7));  // 输出: Max of 4, 7: 7
  ```

- **注意：** 宏没有类型检查，展开后直接替换文本。例如，`SQUARE(1 + 2)` 会展开为 `((1 + 2) * (1 + 2))`。

#### **3. 多行宏**

如果宏的逻辑较为复杂，可以使用反斜杠 `\` 将宏定义为多行。

```c
#define PRINT_VALUES(a, b)        \
  printf("Value A: %d\n", a);     \
  printf("Value B: %d\n", b);
```

- 使用：

  ```c
  PRINT_VALUES(3, 5);
  // 输出:
  // Value A: 3
  // Value B: 5
  ```

#### **4. 条件宏**

条件宏通过预处理指令实现条件编译。

```c
#ifdef DEBUG
  #define LOG(msg) printf("DEBUG: %s\n", msg)
#else
  #define LOG(msg)
#endif
```

- 使用：

  ```c
  LOG("This is a debug message.");
  // 如果定义了 DEBUG，则输出: DEBUG: This is a debug message.
  // 如果没有定义 DEBUG，则什么也不会输出。
  ```

------

### **宏的使用场景**

1. **常量定义**：

   - 使用宏定义常量值，例如 `#define PI 3.14159`。

2. **代码复用**：

   - 定义重复使用的代码块，例如 `#define SQUARE(x) ((x) * (x))`。

3. **条件编译**：

   - 控制代码的编译，适应不同平台或环境，例如：

     ```c
     #ifdef WINDOWS
       printf("Running on Windows\n");
     #endif
     ```

4. **错误处理**：

   - 使用宏简化常见的错误检查逻辑，例如：

     ```c
     #define LASSERT(args, cond, err) \
       if (!(cond)) { lval_del(args); return lval_err(err); }
     ```

------

### **宏的注意事项**

1. **括号的使用**：

   - 在宏中使用参数时，需要用括号包裹参数，避免优先级问题。

   - 例如：

     ```c
     #define SQUARE(x) ((x) * (x))
     ```

2. **调试困难**：

   - 宏展开是预处理阶段完成的，调试器无法跟踪宏内部的逻辑。

3. **滥用可能导致错误**：

   - 宏缺乏类型检查，容易引发意料之外的行为。

4. **避免冗长复杂的宏**：

   - 宏代码过于复杂会降低代码的可读性和可维护性。

------

### **示例：LASSERT 的实现**

#### **宏定义**

```c
#define LASSERT(args, cond, err) \
  if (!(cond)) { lval_del(args); return lval_err(err); }
```

#### **宏展开逻辑**

- 如果 cond 不成立（即条件失败），宏会：
  1. 释放参数 `args`。
  2. 返回一个错误值 `lval_err(err)`。

#### **使用场景**

在 Lisp 风格解释器中，用于简化错误检查：

```c
lval* eval(lval* v) {
  LASSERT(v, v->type == LVAL_NUM, "Expected a number!");
  // 只有在 v 是 LVAL_NUM 时继续执行
  return v;
}
```

展开后代码相当于：

```c
lval* eval(lval* v) {
  if (!(v->type == LVAL_NUM)) {
    lval_del(v);
    return lval_err("Expected a number!");
  }
  return v;
}
```

------

### **总结**

1. **宏是 C 语言中的强大工具**：
   - 它允许在编译前生成代码，减少代码重复，提高效率。
2. **灵活但需谨慎使用**：
   - 宏的灵活性带来便利，但没有类型检查，容易出错，需要遵循规范（例如使用括号包裹参数）。
3. **经典应用**：
   - 宏广泛用于定义常量、复用代码块、条件编译和简化错误处理。

通过正确使用宏，C 语言程序可以变得更加高效和简洁。



代码：[https://github.com/orangeduck/BuildYourOwnLisp/blob/master/src/q_expressions.c](https://github.com/orangeduck/BuildYourOwnLisp/blob/master/src/q_expressions.c)



测试代码：

```c
cc -std=c99 -Wall q_expressions.c mpc.c -ledit -o q_expressions   
./q_expressions 
Lispy Version 0.0.0.0.6
Press Ctrl+c to Exit

lispy> list 1 2 3 4
{1 2 3 4}
lispy> {head (list 1 2 3 4)}
{head (list 1 2 3 4)}
lispy> eval {head (list 1 2 3 4)}
{1}
lispy> tail {tail tail tail}
{tail tail}
lispy> eval (tail {tail tail {5 6 7}})
{6 7}
lispy> eval (head {(+ 1 2) (+ 10 20)})
3
lispy> 
```





