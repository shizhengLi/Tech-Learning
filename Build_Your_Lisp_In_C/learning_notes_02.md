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

