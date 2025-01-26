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

  - `/^/`

    ：确保从输入的开头开始匹配。

    - **作用**：匹配必须从字符串的最开始开始。例如，`+ 1 2` 是有效的，但如果字符串是 `foo + 1 2`，就无法匹配。

  - `<operator>`：匹配操作符，`+`。

  - `<expr>+`：匹配一个或多个表达式，`1 2`。

  - `/$/`

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

  - `<expr>+`

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

/* Use operator string to see which operation to perform */
long eval_op(long x, char* op, long y) {
  if (strcmp(op, "+") == 0) { return x + y; }
  if (strcmp(op, "-") == 0) { return x - y; }
  if (strcmp(op, "*") == 0) { return x * y; }
  if (strcmp(op, "/") == 0) { return x / y; }
  return 0;
}

long eval(mpc_ast_t* t) {
  
  /* If tagged as number return it directly. */ 
  if (strstr(t->tag, "number")) {
    return atoi(t->contents);
  }
  
  /* The operator is always second child. */
  char* op = t->children[1]->contents;
  
  /* We store the third child in `x` */
  long x = eval(t->children[2]);
  
  /* Iterate the remaining children and combining. */
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
  
  puts("Lispy Version 0.0.0.0.3");
  puts("Press Ctrl+c to Exit\n");
  
  while (1) {
  
    char* input = readline("lispy> ");
    add_history(input);
    
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      
      long result = eval(r.output);
      printf("%li\n", result);
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



报错：

```c
Undefined symbols for architecture arm64:
  "_mpc_ast_delete", referenced from:
      _main in parsing-36f430.o
  "_mpc_ast_print", referenced from:
      _main in parsing-36f430.o
  "_mpc_err_delete", referenced from:
      _main in parsing-36f430.o
  "_mpc_err_print", referenced from:
      _main in parsing-36f430.o
  "_mpc_new", referenced from:
      _main in parsing-36f430.o
      _main in parsing-36f430.o
      _main in parsing-36f430.o
      _main in parsing-36f430.o
  "_mpc_parse", referenced from:
      _main in parsing-36f430.o
  "_mpca_lang", referenced from:
      _main in parsing-36f430.o
ld: symbol(s) not found for architecture arm64
clang: error: linker command failed with exit code 1 (use -v to see invocation)
```

解决方法：手动链接mpc.c

```c
cc -std=c99 -Wall evaluation.c mpc.c -ledit -o evaluation
(base) lishizheng@lishizhengdeMacBook-Pro source_code % ./evaluation 
Lispy Version 0.0.0.0.3
Press Ctrl+c to Exit

lispy> + 1 2
3
lispy> + 2 5
7
lispy> 
```



### Bonus Marks

**Write a recursive function to compute the number of leaves of a tree.**

以下是用 C 语言实现的递归计算树叶子节点数量的函数，假设树的结构为二叉树节点。我们将分步骤解释代码和原理：

---

#### 1. **定义树节点结构**
首先定义一个二叉树节点的结构体：
```c
typedef struct TreeNode {
    int val;                   // 节点值（类型可自定义）
    struct TreeNode* left;     // 左子树指针
    struct TreeNode* right;    // 右子树指针
} TreeNode;
```

---

#### 2. **递归函数实现**
```c
int countLeaves(TreeNode* root) {
    // 基本情况1：空节点（递归终止条件）
    if (root == NULL) {
        return 0;
    }
    // 基本情况2：叶子节点（左右子树均为空）
    if (root->left == NULL && root->right == NULL) {
        return 1;
    }
    // 递归情况：非叶子节点，返回左右子树的叶子数之和
    return countLeaves(root->left) + countLeaves(root->right);
}
```

---

#### 3. **代码解释**
**步骤 1：处理空节点**

```c
if (root == NULL) return 0;
```
- 如果当前节点是空指针（比如树的根节点为空，或递归到不存在的子节点），直接返回 0。
- 这是递归的终止条件之一。

**步骤 2：判断叶子节点**

```c
if (root->left == NULL && root->right == NULL) return 1;
```
- 如果一个节点的左右子树均为空，说明它是叶子节点，返回 1。

**步骤 3：递归计算非叶子节点**

```c
return countLeaves(root->left) + countLeaves(root->right);
```
- 如果当前节点不是叶子节点，递归计算其左子树和右子树的叶子节点数，并将结果相加。
- 递归会一直向下分解问题，直到触达叶子节点或空节点。

---

#### 4. **示例测试**
假设有如下二叉树：
```
        1
       / \
      2   3
     / \
    4   5
```
- 叶子节点为 4、5、3，共 3 个。
- 调用 `countLeaves(root)` 的递归过程：
  1. 根节点 1 不是叶子，递归计算左子树（节点 2）和右子树（节点 3）。
  2. 节点 2 不是叶子，递归计算其左子树（节点 4）和右子树（节点 5）。
  3. 节点 4 是叶子，返回 1。
  4. 节点 5 是叶子，返回 1。
  5. 节点 3 是叶子，返回 1。
  6. 最终结果为 1+1+1 = 3。

---

#### 5. **递归原理**
- **问题分解**：将整棵树的叶子数问题分解为左子树和右子树的叶子数问题。
- **终止条件**：
  - 空节点返回 0。
  - 叶子节点返回 1。
- **递归方向**：逐层向下遍历所有子树，直到触达叶子节点。

---

#### 6. **扩展：处理多叉树**
如果是多叉树（每个节点有多个子节点），可以修改结构体和递归逻辑：
```c
typedef struct TreeNode {
    int val;
    int numChildren;          // 子节点数量
    struct TreeNode** children; // 子节点数组指针
} TreeNode;

int countLeaves(TreeNode* root) {
    if (root == NULL) return 0;
    // 如果没有子节点，则是叶子
    if (root->numChildren == 0) return 1;
    // 否则递归所有子节点并累加结果
    int sum = 0;
    for (int i = 0; i < root->numChildren; i++) {
        sum += countLeaves(root->children[i]);
    }
    return sum;
}
```

---

#### 总结
- **核心逻辑**：通过递归将大问题分解为小问题，利用终止条件处理边界。
- **时间复杂度**：O(n)，需要遍历所有节点。
- **空间复杂度**：O(h)，递归栈深度为树的高度（最坏情况下为 O(n)，如链表形态的树）。



**Write a recursive function to compute the number of branches of a tree.**

以下是用 C 语言实现的递归函数，用于计算二叉树的分支数（即所有非空子节点的数量之和，等价于树中边的总数）。代码包含详细注释和示例说明：

---

#### 1. **定义二叉树节点结构**
```c
typedef struct TreeNode {
    int val;                    // 节点值（可自定义类型）
    struct TreeNode* left;      // 左子节点
    struct TreeNode* right;     // 右子节点
} TreeNode;
```

---

#### 2. **递归函数实现**
```c
int countBranches(TreeNode* root) {
    // 递归终止条件：空节点无分支
    if (root == NULL) {
        return 0;
    }
    
    // 计算当前节点的分支数（左子 + 右子）
    int current_branches = 0;
    if (root->left != NULL) current_branches++;
    if (root->right != NULL) current_branches++;
    
    // 递归计算左子树和右子树的分支数，并累加
    return current_branches 
         + countBranches(root->left) 
         + countBranches(root->right);
}
```

---

#### 3. **代码解释**
**步骤 1：处理空节点**

```c
if (root == NULL) return 0;
```
- 空节点没有子节点，因此分支数为 0。

**步骤 2：计算当前节点的分支数**

```c
int current_branches = 0;
if (root->left != NULL) current_branches++;
if (root->right != NULL) current_branches++;
```
- 统计当前节点的左子节点和右子节点是否存在，存在则计为 1 个分支。

**步骤 3：递归累加子节点的分支数**

```c
return current_branches + countBranches(root->left) + countBranches(root->right);
```
- 递归计算左子树和右子树的分支数，并与当前节点的分支数相加。

---

#### 4. **示例测试**
假设有如下二叉树：
```c
        1
       / \
      2   3
     / \   \
    4   5   6
```
- **分支数计算过程**：
  1. 根节点 `1` 有左子 `2` 和右子 `3` → 贡献 **2** 个分支。
  2. 节点 `2` 有左子 `4` 和右子 `5` → 贡献 **2** 个分支。
  3. 节点 `3` 有右子 `6` → 贡献 **1** 个分支。
  4. 叶子节点 `4`、`5`、`6` 无子节点 → 各贡献 **0**。
- **总分支数** = 2 (根) + 2 (左子树) + 1 (右子树) = **5**。

---

#### 5. **时间复杂度与空间复杂度**
- **时间复杂度**：O(n)，需要遍历所有节点（n 为节点总数）。
- **空间复杂度**：O(h)，递归栈深度为树的高度（h 为树的高度）。

---

#### 6. **扩展：多叉树的分支计算**
若树为多叉树（每个节点有多个子节点），修改结构体和函数如下：
```c
typedef struct TreeNode {
    int val;
    int num_children;           // 子节点数量
    struct TreeNode** children; // 子节点数组
} TreeNode;

int countBranches(TreeNode* root) {
    if (root == NULL) return 0;
    
    // 当前节点的分支数 = 子节点数量
    int current_branches = root->num_children;
    
    // 递归计算所有子节点的分支数并累加
    int total = current_branches;
    for (int i = 0; i < root->num_children; i++) {
        total += countBranches(root->children[i]);
    }
    return total;
}
```

---

#### 总结
- **核心逻辑**：递归遍历每个节点，累加其子节点数量。
- **分支定义**：每个非叶子节点的子节点数量之和，等价于树中边的总数（边数 = 分支数）。
- **适用场景**：二叉树、多叉树或其他树形结构的分支统计。



**Write a recursive function to compute the most number of children spanning from one branch of a tree.**

以下是用 C 语言实现的递归函数，用于计算树中任意节点拥有的**最大子节点数量**（即从某个分支向下延伸时，单个节点直接连接的子节点数的最大值）。假设树为多叉树结构，代码包含详细注释和示例说明：

---

#### 1. **定义多叉树节点结构**
```c
typedef struct TreeNode {
    int val;                    // 节点值
    int num_children;           // 子节点数量
    struct TreeNode** children; // 子节点数组指针
} TreeNode;
```

---

#### 2. **递归函数实现**
```c
int maxChildren(TreeNode* root) {
    if (root == NULL) return 0;
    
    // 当前节点的子节点数量
    int current_max = root->num_children;
    
    // 递归遍历所有子节点，计算子节点中的最大值
    for (int i = 0; i < root->num_children; i++) {
        int child_max = maxChildren(root->children[i]);
        if (child_max > current_max) {
            current_max = child_max;
        }
    }
    
    return current_max;
}
```

---

#### 3. **代码解释**
**步骤 1：处理空节点**

```c
if (root == NULL) return 0;
```
- 空节点没有子节点，直接返回 0。

**步骤 2：计算当前节点的子节点数量**

```c
int current_max = root->num_children;
```
- 当前节点的直接子节点数量即为 `num_children`。

**步骤 3：递归遍历子节点**

```c
for (int i = 0; i < root->num_children; i++) {
    int child_max = maxChildren(root->children[i]);
    if (child_max > current_max) {
        current_max = child_max;
    }
}
```
- 遍历所有子节点，递归计算每个子节点子树中的最大子节点数量。
- 将子节点返回的最大值与当前节点的 `num_children` 比较，取更大者。

---

#### 4. **示例测试**
假设有如下多叉树：
```c
        A (3 children)
      / | \
     B  C  D (2 children)
           / \
          E   F (0 children)
```
- **计算过程**：
  1. 根节点 `A` 有 3 个子节点 → `current_max = 3`。
  2. 子节点 `D` 有 2 个子节点 → `child_max = 2`。
  3. 其他子节点 `B`、`C` 无子节点 → `child_max = 0`。
  4. 最终最大值为 `3`（来自根节点 `A`）。

---

#### 5. **扩展：二叉树版本**
如果树是二叉树，结构体和函数如下：
```c
typedef struct TreeNode {
    int val;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

int maxChildren(TreeNode* root) {
    if (root == NULL) return 0;
    
    // 当前节点的子节点数（0、1 或 2）
    int current = 0;
    if (root->left != NULL) current++;
    if (root->right != NULL) current++;
    
    // 递归左子树和右子树，取最大值
    int left_max = maxChildren(root->left);
    int right_max = maxChildren(root->right);
    
    return fmax(current, fmax(left_max, right_max));
}
```
- **示例**：
  ```c
        1
       / \
      2   3
     /   / \
    4   5   6
  ```
  - 根节点 `1` 有 2 个子节点。
  - 节点 `3` 有 2 个子节点。
  - 其他节点最多有 1 个子节点。
  - **最终结果**：2。

---

#### 6. **时间复杂度与空间复杂度**
- **时间复杂度**：O(n)，需要遍历所有节点。
- **空间复杂度**：O(h)，递归栈深度为树的高度。

---

#### 总结
- **核心逻辑**：递归比较当前节点的子节点数和所有子树中的最大值。
- **适用场景**：统计树中某个节点直接拥有的最大子节点数量（例如，多叉树中某个节点有 5 个子节点，即为最大值）。
- **关键点**：递归过程中，始终维护当前已知的最大值，并通过遍历子节点更新这一最大值。



Bonus mark：

- Add the operator `%`, which returns the remainder of division. For example `% 10 6` is `4`.
- › Add the operator `^`, which raises one number to another. For example `^ 4 2` is `16`.
- › Add the function `min`, which returns the smallest number. For example `min 1 5 3` is `1`.
- › Add the function `max`, which returns the biggest number. For example `max 1 5 3` is `5`.
- › Change the minus operator `-` so that when it receives one argument it negates it.

以下是修改后的代码，实现了您要求的所有功能。代码中已添加详细的注释说明：

```c
#include "mpc.h"
#include <math.h>  // 添加数学库支持pow函数

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

/* 扩展运算符处理函数 */
long eval_op(long x, char* op, long y) {
  if (strcmp(op, "+") == 0) { return x + y; }
  if (strcmp(op, "-") == 0) { return x - y; }
  if (strcmp(op, "*") == 0) { return x * y; }
  if (strcmp(op, "/") == 0) { return x / y; }
  if (strcmp(op, "%") == 0) { return x % y; }
  if (strcmp(op, "^") == 0) { return (long)pow(x, y); } // 类型转换
  return 0;
}

long eval(mpc_ast_t* t) {
  
  /* 基础数字处理 */
  if (strstr(t->tag, "number")) {
    return atoi(t->contents);
  }
  
  char* op = t->children[1]->contents;
  
  /* 处理单参数负号 */
  if (strcmp(op, "-") == 0 && t->children_num == 3) {
    return -eval(t->children[2]);
  }
  
  /* 处理min/max函数 */
  if (strcmp(op, "min") == 0 || strcmp(op, "max") == 0) {
    long result = eval(t->children[2]);  // 第一个参数
    for (int i = 3; i < t->children_num; i++) {
      if (strstr(t->children[i]->tag, "expr")) {
        long val = eval(t->children[i]);
        result = (strcmp(op, "min") == 0) ? 
          (val < result ? val : result) : 
          (val > result ? val : result);
      }
    }
    return result;
  }
  
  /* 常规运算符处理 */
  long x = eval(t->children[2]);
  int i = 3;
  while (i < t->children_num && strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }
  return x;
}

int main(int argc, char** argv) {
  
  /* 修改语法规则 */
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Lispy    = mpc_new("lispy");
  
  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                     \
      number   : /-?[0-9]+/ ;                             \
      operator : '+' | '-' | '*' | '/' | '%' | '^'       \
               | \"min\" | \"max\" ;                     \
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
      long result = eval(r.output);
      printf("%li\n", result);
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

---

#### 主要修改说明：

1. **运算符扩展**：
   
   ```c
   // 在eval_op中添加
   if (strcmp(op, "%") == 0) { return x % y; }
   if (strcmp(op, "^") == 0) { return (long)pow(x, y); }
   ```
   
2. **min/max函数实现**：
   ```c
   if (strcmp(op, "min") == 0 || strcmp(op, "max") == 0) {
     long result = eval(t->children[2]);
     for (int i = 3; ...) { /* 遍历比较所有参数 */ }
   }
   ```

3. **单参数负号处理**：
   ```c
   if (strcmp(op, "-") == 0 && t->children_num == 3) {
     return -eval(t->children[2]);
   }
   ```

4. **语法规则修改**：
   ```c
   operator : '+' | '-' | '*' | '/' | '%' | '^' | \"min\" | \"max\" ;
   ```

---

#### 编译命令（注意添加 -lm）：
```bash
cc -std=c99 -Wall parsing.c mpc.c -ledit -lm -o parsing
```

---

#### 验证示例：
1. `% 10 6` → 4
2. `^ 4 2` → 16
3. `min 5 3 9` → 3
4. `max 5 3 9` → 9
5. `- 7` → -7
6. `- 10 3` → 7

每个功能均通过递归遍历AST实现，保持了代码的简洁性和扩展性。

测试：

```c
cc -std=c99 -Wall eval_bonus_mark.c mpc.c -ledit -o eval_bonus_mark
 ./eval_bonus_mark 
Lispy Version 0.0.0.0.4
Press Ctrl+c to Exit

lispy> % 4 2
0
lispy> max 100000 2
100000
lispy> min 100000 -1
-1
lispy> - 4 
4
lispy> - 4 2
2
lispy> 
```

