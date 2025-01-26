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

