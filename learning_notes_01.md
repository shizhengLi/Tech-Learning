# Build Your Own Lisp

Learn C and build your own programming language in 1000 lines of code!

Link: [https://www.buildyourownlisp.com/](https://www.buildyourownlisp.com/)

Link: [https://www.buildyourownlisp.com/contents](https://www.buildyourownlisp.com/contents)

Source code: [https://github.com/orangeduck/BuildYourOwnLisp/tree/master/src](https://github.com/orangeduck/BuildYourOwnLisp/tree/master/src)

# Introduction • Chapter 1



# Installation • Chapter 2



```C
gcc --version
Apple clang version 15.0.0 (clang-1500.3.9.4)
Target: arm64-apple-darwin23.3.0
Thread model: posix
InstalledDir: /Library/Developer/CommandLineTools/usr/bin
```





```c
cc -std=c99 -Wall hello_world.c -o hello_world
ls
hello_world     hello_world.c
./hello_world 
Hello, world!
```



# Basics • Chapter 3



# An Interactive Prompt • Chapter 4



This is a program that prompts the user for some input, and when supplied with it, replies back with some message. Using this will be the easiest way to test our programming language and see how it acts. This system is also called a *REPL*, which stands for *read*-*evaluate*-*print* *loop*. 



```c
#include <stdio.h>

/* Declare a buffer for user input of size 2048 */
static char input[2048];

int main(int argc, char** argv) {

  /* Print Version and Exit Information */
  puts("Lispy Version 0.0.0.0.1");
  puts("Press Ctrl+c to Exit\n");

  /* In a never ending loop */
  while (1) {

    /* Output our prompt */
    fputs("lispy> ", stdout);

    /* Read a line of user input of maximum size 2048 */
    fgets(input, 2048, stdin);

    /* Echo input back to user */
    printf("No you're a %s", input);
  }

  return 0;
}
```



编译测试：

```c
cc -std=c99 -Wall prompt.c -o prompt

 ./prompt 
Lispy Version 0.0.0.0.1
Press Ctrl+c to Exit

lispy> hello
No you're a hello
lispy> my name is shizheng
No you're a my name is shizheng
lispy> 
```



```c
#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>

int main(int argc, char** argv) {

  /* Print Version and Exit Information */
  puts("Lispy Version 0.0.0.0.1");
  puts("Press Ctrl+c to Exit\n");

  /* In a never ending loop */
  while (1) {

    /* Output our prompt and get input */
    char* input = readline("lispy> ");

    /* Add input to history */
    add_history(input);

    /* Echo input back to user */
    printf("No you're a %s\n", input);

    /* Free retrieved input */
    free(input);

  }

  return 0;
}
```



遇到问题：

```c
cc -std=c99 -Wall prompt.c -o prompt

Undefined symbols for architecture arm64:
  "_add_history", referenced from:
      _main in prompt-ba9bda.o
  "_readline", referenced from:
      _main in prompt-ba9bda.o
ld: symbol(s) not found for architecture arm64
clang: error: linker command failed with exit code 1 (use -v to see invocation)
```

This means that you haven't *linked* your program to `editline`. This *linking* process allows the compiler to directly embed calls to `editline` in your program. You can make it link by adding the flag `-ledit` to your compile command, just before the output flag.

使用

```c
cc -std=c99 -Wall prompt.c -ledit -o prompt
```

测试

```c
./prompt 
Lispy Version 0.0.0.0.1
Press Ctrl+c to Exit

lispy> hello
No you're a hello
lispy> ni
No you're a ni
lispy> good 
No you're a good 
lispy> bird
No you're a bird
lispy> cat
No you're a cat
lispy> 
```



# Languages • Chapter 5





# Parsing • Chapter 6

Polish Notation：前缀表达式



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
// #include <editline/history.h>
#endif

int main(int argc, char** argv) {
  
  /* Create Some Parsers */
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Lispy    = mpc_new("lispy");
  
  /* Define them with the following Language */
  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                     \
      number   : /-?[0-9]+/ ;                             \
      operator : '+' | '-' | '*' | '/' ;                  \
      expr     : <number> | '(' <operator> <expr>+ ')' ;  \
      lispy    : /^/ <operator> <expr>+ /$/ ;             \
    ",
    Number, Operator, Expr, Lispy);
  
  puts("Lispy Version 0.0.0.0.2");
  puts("Press Ctrl+c to Exit\n");
  
  while (1) {
  
    char* input = readline("lispy> ");
    add_history(input);
    
    /* Attempt to parse the user input */
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      /* On success print and delete the AST */
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    } else {
      /* Otherwise print and delete the Error */
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    
    free(input);
  }
  
  /* Undefine and delete our parsers */
  mpc_cleanup(4, Number, Operator, Expr, Lispy);
  
  return 0;
}
```



测试代码：

```c
./parsing 
Lispy Version 0.0.0.0.2
Press Ctrl+c to Exit

lispy> + 5 (* 2 2)
> 
  regex 
  operator|char:1:1 '+'
  expr|number|regex:1:3 '5'
  expr|> 
    char:1:5 '('
    operator|char:1:6 '*'
    expr|number|regex:1:8 '2'
    expr|number|regex:1:10 '2'
    char:1:11 ')'
  regex 
lispy> * 20 (+ 3 5 )
> 
  regex 
  operator|char:1:1 '*'
  expr|number|regex:1:3 '20'
  expr|> 
    char:1:6 '('
    operator|char:1:7 '+'
    expr|number|regex:1:9 '3'
    expr|number|regex:1:11 '5'
    char:1:13 ')'
  regex 
lispy> hello
<stdin>:1:1: error: expected '+', '-', '*' or '/' at 'h'
lispy> 1 dog
<stdin>:1:1: error: expected '+', '-', '*' or '/' at '1'
lispy> 
```



## 解释正则表达式



这段代码中的 `mpca_lang` 定义了一种简单的上下文无关文法（CFG），并使用正则表达式来描述每个非终结符（例如 `number` 和 `operator`）。下面逐行解释该文法及其正则表达式的含义：

------

### **正则表达式和语言规则**

#### 1. `number : /-?[0-9]+/ ;`

- **定义**: `number` 表示一个整数。
- 正则表达式: `/` 包围的部分是正则表达式。
  - `-?`：可选的负号，表示数字可以是负数（`-` 出现 0 次或 1 次）。
  - `[0-9]+`：一个或多个数字字符。
- **总结**: 匹配所有整数，包括负数。例如：`123`，`-456`。

------

#### 2. `operator : '+' | '-' | '*' | '/' ;`

- **定义**: `operator` 是一个操作符。
- 符号解释:
  - `'+'`、`'-'`、`'*'` 和 `'/'` 是四个操作符，表示加、减、乘、除。
  - `|` 是 "或者" 的逻辑符号，表示匹配这四个中的任意一个。
- **总结**: 匹配加减乘除操作符中的一个。例如：`+`，`*`。

------

#### 3. `expr : <number> | '(' <operator> <expr>+ ')' ;`

- **定义**: `expr` 表示一个表达式。
- 语法:
  - `<number>`：一个数字。
  - `'(' <operator> <expr>+ ')'`：
    - `'('` 和 `')'`：左括号和右括号，表示括起来的表达式。
    - `<operator>`：括号内的第一个内容是操作符。
    - `<expr>+`：一个或多个表达式（`+` 表示至少一个 `<expr>`）。
- 总结:
  - 匹配单独的一个数字。
  - 或者匹配括号中的一个操作符，后跟一个或多个表达式。例如：
    - `123`（单独数字）
    - `(+ 1 2)`（加法表达式）
    - `(* 2 (+ 1 3))`（嵌套表达式）。

------

#### 4. `lispy : /^/ <operator> <expr>+ /$/ ;`

- **定义**: `lispy` 是完整的 Lispy 表达式。
- 语法:
  - `/^/`：匹配输入的开头。
  - `<operator>`：起始为操作符。
  - `<expr>+`：一个或多个表达式。
  - `/$/`：匹配输入的结尾。
- 总结: 整个输入是一个完整的 Lispy 表达式。例如：
  - `+ 1 2`（加法）
  - `* 3 (+ 1 2)`（乘法和嵌套加法）。

------

### **文法的整体作用**

这段定义描述了一个简单的 Lisp 风格的语言，其中：

- **数字**: 表示基础的数字值。
- **操作符**: 定义了支持的四种数学运算。
- **表达式**: 可以是数字，也可以是包含操作符的复杂表达式。
- **Lispy**: 表示完整的输入必须是操作符开头、一个或多个表达式结尾的结构。

### **正则表达式总结**

正则表达式通过简单的规则描述了如何匹配基本单元（数字、操作符），再通过递归规则定义了复杂结构（表达式）。这为解析类似 Lisp 的语言提供了一个简单的基础。



## 正则表达式中的 `^` 和 `$` 



正则表达式中的 `^` 和 `$` 分别用来匹配输入的开头和结尾。这两个符号的作用是确保匹配的内容覆盖了整个输入字符串，而不是只匹配部分内容。

------

### 示例详解

#### 1. 表达式：`+ 1 2`

- **正则表达式分解**：

  - ```
    /^/
    ```

    ：确保从输入的开头开始匹配。

    - **作用**：匹配必须从字符串的最开始开始。例如，`+ 1 2` 是有效的，但如果字符串是 `foo + 1 2`，就无法匹配。

  - `<operator>`：匹配操作符，`+`。

  - `<expr>+`：匹配一个或多个表达式，`1 2`。

  - ```
    /$/
    ```

    ：确保匹配到字符串的结尾。

    - **作用**：匹配必须到字符串的末尾结束。例如，`+ 1 2` 是有效的，但如果字符串是 `+ 1 2 bar`，就无法匹配。

- **完整匹配过程**：

  1. `/^/` 匹配字符串的开头。
  2. `<operator>` 匹配 `+`。
  3. `<expr>+` 匹配 `1` 和 `2`。
  4. `/$/` 确保匹配到输入的结尾。

  - 结果：整个字符串 `+ 1 2` 是一个完整的 Lispy 表达式。

------

#### 2. 表达式：`* 3 (+ 1 2)`

- **正则表达式分解**：

  - `/^/`：确保从输入的开头开始匹配。

  - `<operator>`：匹配操作符，`*`。

  - ```
    <expr>+
    ```

    ：匹配一个或多个表达式。

    - `3` 是一个表达式。
    - `(+ 1 2)` 是另一个嵌套表达式。

  - `/$/`：确保匹配到输入的结尾。

- **完整匹配过程**：

  1. `/^/` 匹配字符串的开头。
  2. `<operator>` 匹配 `*`。
  3. `<expr>+` 依次匹配 `3` 和 `(+ 1 2)`。
  4. `/$/` 确保匹配到输入的结尾。

  - 结果：整个字符串 `* 3 (+ 1 2)` 是一个完整的 Lispy 表达式。

------

### 为什么要匹配开头和结尾？

#### 不匹配开头和结尾的风险：

如果没有 `/^/` 和 `/$/`，正则表达式会匹配到字符串中的任意部分，例如：

- 输入字符串是 `foo + 1 2 bar`。
- 没有 `/^/` 和 `/$/` 时，正则表达式会匹配 `+ 1 2`，忽略 `foo` 和 `bar`。

#### 匹配开头和结尾的好处：

使用 `/^/` 和 `/$/`，正则表达式要求整个字符串都符合定义的 Lispy 表达式：

- `foo + 1 2 bar` 不会匹配。
- 只有 `+ 1 2` 或 `* 3 (+ 1 2)` 这样的完整表达式才会匹配。

------

### 关键点总结

- `/^/` 和 `/$/` 的目的是确保正则表达式匹配整个输入，而不是字符串中的一部分。
- 这可以保证输入的合法性，防止用户输入多余的或无效的字符。



## foo + 1 2 bar 为什么不会匹配



对于文法定义：

```c
lispy : /^/ <operator> <expr>+ /$/ ;
```

匹配规则要求 **整个输入字符串** 从开头到结尾都必须符合 Lispy 表达式的结构。下面详细解释为什么 `foo + 1 2 bar` 不会匹配。

------

### **匹配规则回顾**

1. `/^/`：
   - 表示正则匹配必须从字符串的开头开始。
   - **在 `foo + 1 2 bar` 中，开头是 `foo`，而不是一个操作符**，因此不匹配。
2. `<operator>`：
   - 要求第一个有效字符是一个操作符 (`+`, `-`, `*`, `/`)。
   - 在 `foo + 1 2 bar` 中，`foo` 不是有效的操作符，导致无法匹配。
3. `<expr>+`：
   - 要求有一个或多个表达式。
   - 即使后面存在合法的表达式（如 `1` 和 `2`），正则匹配在开头（`foo`）失败后不会继续检查。
4. `/$/`：
   - 表示匹配必须一直到字符串的结尾。
   - 即使开头匹配正确，尾部的 `bar` 也不符合 Lispy 表达式定义，所以匹配会失败。

------

### **匹配过程（为何失败）**

对于输入 `foo + 1 2 bar`：

1. **第一步** `/^/` 匹配字符串开头：
   - 开头是 `foo`，而 `<operator>` 要求第一个字符是 `+`, `-`, `*`, 或 `/`。
   - 由于 `foo` 不符合 `<operator>` 的规则，匹配直接失败。
2. **中间部分** `+ 1 2`：
   - 虽然中间部分看似符合 `<operator>` 和 `<expr>` 的规则，但正则表达式要求整个输入字符串符合定义。
   - 因此，即使中间部分合法，也无法通过匹配。
3. **最后一步** `/$/` 匹配字符串结尾：
   - 即使前面都匹配成功，但 `bar` 不符合文法定义的 `<expr>`，所以结尾也会失败。

------

### **合法输入示例**

以下是一些可以完整匹配的输入：

1. `+ 1 2`：
   - 开头是 `+`（操作符），后面是两个表达式 `1` 和 `2`。
   - 符合从开头到结尾的规则。
2. `* 3 (+ 1 2)`：
   - 开头是 `*`，后面是一个表达式 `3` 和一个嵌套表达式 `(+ 1 2)`。
   - 符合规则。

------

### **总结**

输入 `foo + 1 2 bar` 不会匹配是因为：

1. 开头 `foo` 不是合法的 `<operator>`。
2. 尾部 `bar` 不是合法的 `<expr>`。
3. 整个字符串没有从开头到结尾完全符合 Lispy 表达式的定义。



# Evaluation • Chapter 7



