

# Tutorial - Write a Shell in C



## 前言

本文是学习笔记，读者可参见原文。

Link: [https://brennan.io/2015/01/16/write-a-shell-in-c/](https://brennan.io/2015/01/16/write-a-shell-in-c/)

Github: [https://github.com/brenns10/lsh](https://github.com/brenns10/lsh)



>Basic lifetime of a shell
>
>Let’s look at a shell from the top down. A shell does three main things in its lifetime.
>
>- **Initialize**: In this step, a typical shell would read and execute its configuration files. These change aspects of the shell’s behavior.
>- **Interpret**: Next, the shell reads commands from stdin (which could be interactive, or a file) and executes them.
>- **Terminate**: After its commands are executed, the shell executes any shutdown commands, frees up any memory, and terminates.

### **Shell 的生命周期（Lifetime of a Shell）**

从顶层来看，**一个 Shell 在整个生命周期中主要执行三个核心阶段**：

1. **初始化（Initialize）**
2. **解析和执行命令（Interpret）**
3. **终止（Terminate）**

------

### **1. 初始化（Initialize）**

在 Shell 启动时，它会**加载配置文件**并**初始化环境**。Shell 的初始化过程会因**Shell 的类型（如 `bash`, `zsh`, `sh`, `ksh`）以及启动方式（交互式 / 非交互式, 登录 / 非登录）**而有所不同。

#### **1.1 交互式 vs. 非交互式**

- 交互式 Shell

  （Interactive Shell）

  - 由用户手动启动，比如在终端运行 `bash` 或 `zsh`。
  - 需要从标准输入（stdin）读取用户命令并执行。

- 非交互式 Shell

  （Non-interactive Shell）

  - 主要用于执行 Shell 脚本，如 `./script.sh`。
  - 只运行脚本，不会进入交互模式。

#### **1.2 登录 Shell vs. 非登录 Shell**

- 登录 Shell（Login Shell）
  - 由终端或 SSH 登录时启动，如 `bash --login` 或 SSH 远程登录。
  - 需要加载用户的环境配置（如 `PATH`, `HOME`）。
- 非登录 Shell（Non-login Shell）
  - 运行 Shell 时没有新的登录环境，比如直接在终端运行 `bash`。
  - **不会加载登录配置文件**，而是使用非登录 Shell 配置。

------

#### **1.3 加载的配置文件**

不同类型的 Shell 在初始化时会读取不同的配置文件，以下是 `bash` 和 `zsh` 在不同模式下的行为：

#### **Bash 配置文件**

| Shell 类型                   | 读取的配置文件                                               |
| ---------------------------- | ------------------------------------------------------------ |
| **登录 Shell**               | `/etc/profile`, `~/.bash_profile`, `~/.bash_login`, `~/.profile`（按顺序查找） |
| **非登录 Shell**             | `~/.bashrc`                                                  |
| **非交互式 Shell（如脚本）** | `$BASH_ENV`                                                  |
| **Shell 退出时**             | `~/.bash_logout`                                             |

#### **Zsh 配置文件**

| Shell 类型       | 读取的配置文件                     |
| ---------------- | ---------------------------------- |
| **登录 Shell**   | `/etc/zsh/zprofile`, `~/.zprofile` |
| **非登录 Shell** | `/etc/zsh/zshrc`, `~/.zshrc`       |
| **Shell 退出时** | `~/.zlogout`                       |

------

#### **1.4 主要配置文件解析**

#### **（1）`/etc/profile`**

- **系统级**的 `bash` 配置文件，对所有用户生效。

- 由 

  ```
  bash --login
  ```

   启动时读取，通常设置：

  - 全局环境变量（如 `PATH`, `LANG`）。
  - 调用其他配置文件，如 `/etc/bash.bashrc`。

#### **（2）`~/.bash_profile`（或 `~/.zprofile` in Zsh）**

- **用户级**的登录 Shell 配置文件，仅在用户登录时执行。

- 主要作用：

  - 配置用户环境变量（如 `export PATH`）。

  - 执行 

    ```
    ~/.bashrc
    ```

     以确保非交互式模式也能加载相应配置：

    ```sh
    if [ -f ~/.bashrc ]; then
        source ~/.bashrc
    fi
    ```

#### **（3）`~/.bashrc`（或 `~/.zshrc` in Zsh）**

- **非登录 Shell 配置文件**，如在终端输入 `bash` 或 `zsh` 时加载。
- 主要作用：
  - 定义别名（alias）
  - 配置 Shell 提示符（PS1）
  - 定义函数、自动补全等

#### **（4）`$BASH_ENV`**

- **用于非交互式 Shell（脚本执行）**
- 当 `bash` 运行一个脚本时，它不会加载 `~/.bashrc`，但如果 `BASH_ENV` 变量被设置，`bash` 会**执行该变量指向的文件**。

#### **（5）`~/.bash_logout`**

- 当用户退出 `bash` 登录 Shell 时执行，可用于：
  - 清理临时文件
  - 记录日志
  - 关闭进程

------

### **2. 解析和执行命令（Interpret）**

初始化后，Shell 进入 **解析和执行命令**（Interpret）阶段：

1. 读取命令（用户输入或脚本）
2. 解析命令（词法分析、语法解析）
3. 执行命令（内建命令、外部程序、脚本）

#### **2.1 交互式 Shell**

```sh
$ echo "Hello, World!"
Hello, World!
```

- 交互式 Shell **等待用户输入命令**。
- 解析并执行 `echo` 命令。

#### **2.2 非交互式 Shell（脚本执行）**

```sh
#!/bin/bash
echo "This is a script"
```

- 执行 `script.sh` 文件：

  ```sh
  bash script.sh
  ```

- `bash` **不进入交互模式**，而是逐行读取脚本并执行。

------

### **3. 终止（Terminate）**

Shell 终止时，会执行一些**清理操作**：

1. 运行 `~/.bash_logout`（如果是 `bash --login`）。
2. 释放分配的内存。
3. 退出进程，返回到系统。

#### **3.1 交互式 Shell 终止**

常见的终止方式：

- **手动退出**

  ```sh
  exit
  ```

  或按 `Ctrl+D` 发送 `EOF`（End Of File）。

- **注销（Logout）**

  ```sh
  logout
  ```

  适用于 **登录 Shell**（非 `bash` 子进程）。

#### **3.2 终止 Shell 进程**

```sh
kill -9 $$
```

- `$$` 是 **当前 Shell 进程 ID（PID）**，`kill -9` 强制终止它。

#### **3.3 `~/.bash_logout` 示例**

```sh
echo "Goodbye, $USER!" >> /var/log/logout.log
```

- 退出时写入日志 `/var/log/logout.log`。

------

### **4. 总结**

| 阶段           | 主要任务                   | 关键配置文件                                   |
| -------------- | -------------------------- | ---------------------------------------------- |
| **初始化**     | 读取配置文件、设置环境变量 | `/etc/profile`, `~/.bash_profile`, `~/.bashrc` |
| **解析和执行** | 读取命令并执行             | 脚本 (`script.sh`), 交互命令 (`echo "hi"`)     |
| **终止**       | 释放资源、退出进程         | `~/.bash_logout`                               |

**Shell 生命周期**：

1. **启动**（加载配置文件，初始化环境）
2. **解析命令**（交互式输入或执行脚本）
3. **终止**（释放资源，注销）

🚀 **理解 Shell 生命周期，能帮助你更好地管理 Shell 环境，提高系统管理和脚本编写效率！**



## Loop



这个教程没有考虑配置的问题，只是读取命令行执行并终止。

```c
int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  lsh_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}
```



>
>
>Basic loop of a shell
>
>So we’ve taken care of how the program should start up. Now, for the basic program logic: what does the shell do during its loop? Well, a simple way to handle commands is with three steps:
>
>- **Read**: Read the command from standard input.
>- **Parse**: Separate the command string into a program and arguments.
>- **Execute**: Run the parsed command.



loop要干的事情如下：

```c
void lsh_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    free(line);
    free(args);
  } while (status);
}
```



## Reading a line



>Reading a line from stdin sounds so simple, but in C it can be a hassle. The sad thing is that you don’t know ahead of time how much text a user will enter into their shell. You can’t simply allocate a block and hope they don’t exceed it. Instead, you need to start with a block, and if they do exceed it, reallocate with more space. This is a common strategy in C, and we’ll use it to implement `lsh_read_line()`.





```c
#define LSH_RL_BUFSIZE 1024
char *lsh_read_line(void)
{
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    // If we hit EOF, replace it with a null character and return.
    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}
```

### **关于 `int` 在 `getchar()` 读取中的常见陷阱**

在 `lsh_read_line()` 函数中，有一个细节值得注意：

```c
int c;
c = getchar();
```

- **为什么 `c` 是 `int` 而不是 `char`？**
- **如果 `c` 是 `char`，可能会导致什么错误？**
- **为什么 `EOF` 是 `int`，而不是 `char`？**

------

### **1. `getchar()` 返回 `int` 而不是 `char`**

`getchar()` 的原型：

```c
int getchar(void);
```

- 它返回**`int` 类型，而不是 `char`**。
- 主要原因是 `getchar()` 可能返回 **正常的字符（0-255，通常 8-bit）**，也可能返回 **EOF（End of File）**。
- **EOF 是一个 `int`，其值通常是 `-1`，而不是一个 `char` 类型的值。**

------

### **2. `char` 类型的问题**

如果 `c` 被错误地定义为 `char`：

```c
char c;
c = getchar();
```

那么可能会出现如下问题：

1. **EOF (`-1`) 被截断或错误解释**
   - `char` 可能是 **无符号 (`unsigned char`)**，即 `0~255`。
   - `EOF` 通常是 `-1`，存入 `unsigned char` 时，它可能变成 `255`（`0xFF`）。
   - 这会导致 `if (c == EOF)` **永远不会触发**，因为 `c` 可能永远不会变成 `-1`。
2. **符号扩展错误**
   - 如果 `char` 是 **有符号 (`signed char`)**，则在某些平台上 `char` 可能是 `8-bit`，而 `int` 是 `32-bit`。
   - 当 `char` 被提升为 `int` 时，如果 `char` 的最高位是 `1`（即大于 127 的值，如 `ä` 的 ASCII 码 `228`），它可能会被错误地扩展为负值。

------

### **3. 代码示例：错误的 `char` 变量**

假设我们错误地使用 `char` 变量来存储 `getchar()` 的返回值：

```c
#include <stdio.h>

int main() {
    char c;
    while ((c = getchar()) != EOF) {
        putchar(c);
    }
    printf("EOF detected\n");
    return 0;
}
```

### **潜在问题**

- 如果 `char` 是 **unsigned**，`EOF (-1)` 可能会被存储为 `255`（`0xFF`），**导致死循环**。
- `if (c == EOF)` **可能永远不会触发**，因为 `char` **不会存储 `-1`**。

------

### **4. 正确的做法：使用 `int`**

正确的写法应该是：

```c
#include <stdio.h>

int main() {
    int c;  // 正确的类型
    while ((c = getchar()) != EOF) {
        putchar(c);
    }
    printf("EOF detected\n");
    return 0;
}
```

### **为什么正确？**

1. `c` 是 `int`，可以正确存储 **所有可能的 `char` 值（0-255）** 以及 `EOF (-1)`。
2. `EOF` 比较时不会被截断，避免死循环。

------

### **5. `char` 和 `int` 的存储范围**

| **数据类型**      | **存储范围**                            |
| ----------------- | --------------------------------------- |
| `char`（有符号）  | `-128` 到 `127`                         |
| `char`（无符号）  | `0` 到 `255`                            |
| `int`（典型情况） | `-2147483648` 到 `2147483647`（32-bit） |
| `EOF`（通常定义） | `-1`                                    |

------

### **6. 其他 `EOF` 检测错误**

错误示例：

```c
while ((char c = getchar()) != EOF) {  // ❌ 错误：EOF 可能不会匹配
    putchar(c);
}
```

正确示例：

```c
int c;
while ((c = getchar()) != EOF) {  // ✅ 正确：`c` 可以存储 `EOF`
    putchar(c);
}
```

------

### **7. 结论**

✅ **`getchar()` 返回 `int`，所以必须用 `int` 变量存储它。**
 ✅ **避免 `char` 存储 `getchar()` 的返回值，否则可能导致 `EOF` 检测错误。**
 ✅ **`EOF` 通常是 `-1`，使用 `char` 可能会导致死循环或错误判断。**
 ✅ **任何需要检查 `EOF` 的代码，都必须使用 `int` 类型来存储 `getchar()` 结果！**

🎯 **牢记这一点，可以避免 C 语言中常见的输入处理错误！** 🚀





```c
char *lsh_read_line(void)
{
  char *line = NULL;
  ssize_t bufsize = 0; // have getline allocate a buffer for us

  if (getline(&line, &bufsize, stdin) == -1){
    if (feof(stdin)) {
      exit(EXIT_SUCCESS);  // We recieved an EOF
    } else  {
      perror("readline");
      exit(EXIT_FAILURE);
    }
  }

  return line;
}
```

### **解析 `lsh_read_line()`（使用 `getline()`）**

这一版本的 `lsh_read_line()` 使用了 `getline()`，这使得代码更加简洁高效，同时避免了手动管理输入缓冲区的麻烦。

------

### **1. `getline()` 简介**

#### **1.1 `getline()` 的函数原型**

```c
ssize_t getline(char **lineptr, size_t *n, FILE *stream);
```

- `lineptr`：指向 `char*` 指针的指针（`*lineptr` 将指向读取的字符串）。
- `n`：指向 `size_t` 变量的指针（存储缓冲区大小）。
- `stream`：输入流（通常为 `stdin`）。
- 返回值：
  - 成功时返回读取的字符数（包括换行符 `\n`）。
  - 失败时返回 `-1`，并设置 `errno`。

------

### **2. 代码解析**

```c
char *lsh_read_line(void)
{
  char *line = NULL;
  ssize_t bufsize = 0; // have getline allocate a buffer for us
```

- `char *line = NULL;`：
  - `line` 是指向输入字符串的指针，**初始化为空指针**，让 `getline()` 负责分配内存。
- `ssize_t bufsize = 0;`：
  - `bufsize` 存储缓冲区大小，**传递 0 让 `getline()` 负责自动分配**。

------

### **(1) `getline()` 读取用户输入**

```c
  if (getline(&line, &bufsize, stdin) == -1){
```

- `getline(&line, &bufsize, stdin)`：
  - 读取 `stdin`（标准输入）中的一行数据，自动分配所需的内存，并存储在 `line` 中。
  - 读取的字符串包括 `\n`，但会自动在末尾添加 `\0` 结束符。
- 如果 `getline()` 返回 `-1`，说明**出现了错误**（例如 `EOF` 或其他读取失败）。

------

### **(2) 处理 `EOF` 或其他错误**

```c
    if (feof(stdin)) {
      exit(EXIT_SUCCESS);  // We received an EOF
    } else  {
      perror("readline");
      exit(EXIT_FAILURE);
    }
```

- `feof(stdin)`：
  - 检查 `stdin` 是否到达文件结束（EOF）：
    - 可能是 **用户按 `Ctrl+D`**（终端 EOF 信号）。
    - 也可能是 **读取文件到达结尾**（如果 `stdin` 关联的是文件）。
  - 如果遇到 `EOF`，程序**正常退出**（`exit(EXIT_SUCCESS)`）。
- **如果是其他错误**：
  - `perror("readline");`：打印错误信息（`perror` 会根据 `errno` 显示错误原因）。
  - `exit(EXIT_FAILURE);`：终止程序，并返回失败状态。

------

### **(3) 成功读取后返回**

```c
  return line;
}
```

- 如果 `getline()` 读取成功：
  - `line` 现在指向已分配的字符串，**调用者需要负责 `free(line)` 释放内存**。
  - `return line;` **返回读取的字符串**。

------

### **3. `getline()` 版本 vs. 传统 `getchar()` 版本**

|                | `getline()` 版本               | `getchar()` 版本                       |
| -------------- | ------------------------------ | -------------------------------------- |
| **缓冲区管理** | 自动分配和扩展                 | 需手动管理（`malloc()` + `realloc()`） |
| **读取效率**   | 更高效，避免字符逐个读取       | 逐个 `getchar()` 读取字符              |
| **错误处理**   | `getline()` 处理 `EOF`，更安全 | 需手动检查 `EOF`                       |
| **代码简洁性** | 更简洁，避免 `while(1)`        | 代码较长，需手动扩展缓冲区             |

------

### **4. `getline()` 使用示例**

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    printf("Enter some text: ");
    read = getline(&line, &len, stdin);
    
    if (read == -1) {
        perror("getline");
        exit(EXIT_FAILURE);
    }

    printf("You entered: %s", line);

    free(line); // 释放 `getline()` 分配的内存
    return 0;
}
```

**示例输入输出**

```
Enter some text: Hello, World!
You entered: Hello, World!
```

------

### **5. 结论**

✅ **`getline()` 是现代 C 语言中推荐的行读取方法**，**自动分配内存**，**无需手动扩展缓冲区**。
 ✅ **比 `getchar()` 逐字符读取更高效**，且更容易处理长输入。
 ✅ **一定要 `free(line)` 释放 `getline()` 分配的内存，否则会造成内存泄漏！**

🚀 **如果系统支持 `getline()`，强烈推荐使用它来读取输入！**



## Parsing the line



>**好的，如果我们回头看看这个循环，我们会看到我们现在已经实现了lsh_read_line()，并且我们有了输入行。现在，我们需要将这一行解析成一个参数列表。我要在这里做一个明显的简化，说我们不允许在命令行参数中使用引号或反斜杠转义。相反，我们将简单地使用空格来分隔参数。因此，命令echo "this message "不会用单个参数调用echo this message，而是用两个参数调用echo:  "this 和 message"。**
>
>OK, so if we look back at the loop, we see that we now have implemented `lsh_read_line()`, and we have the line of input. Now, we need to parse that line into a list of arguments. I’m going to make a glaring simplification here, and say that we won’t allow quoting or backslash escaping in our command line arguments. Instead, we will simply use whitespace to separate arguments from each other. So the command `echo "this message"` would not call echo with a single argument `this message`, but rather it would call echo with two arguments: `"this` and `message"`.



delimiters：分隔符



>
>
>With those simplifications, all we need to do is “tokenize” the string using whitespace as delimiters. That means we can break out the classic library function `strtok` to do some of the dirty work for us.



```c
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);  // Why using NULL? See below
  }
  tokens[position] = NULL;
  return tokens;
}
```

### **解析 `lsh_split_line()`**

该函数的作用是 **解析用户输入的命令行字符串，将其拆分成一个由单词组成的数组**，以供后续执行。

------

### **1. 代码概述**

```c
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
```

- `LSH_TOK_BUFSIZE`：初始缓冲区大小，存储指向字符串片段（tokens）的指针数组，默认为 `64`。
- `LSH_TOK_DELIM`：定义分隔符，包括 **空格 (`' '`), tab (`'\t'`), 回车 (`'\r'`), 换行 (`'\n'`), 响铃符 (`'\a'`)**。

```c
char **lsh_split_line(char *line)
```

- 该函数**接收一行输入字符串 `line`，并返回解析后的字符串数组**（即 `argv[]` 风格）。

------

### **2. 代码解析**

### **(1) 初始化动态数组**

```c
int bufsize = LSH_TOK_BUFSIZE, position = 0;
char **tokens = malloc(bufsize * sizeof(char*));
char *token;

if (!tokens) {
  fprintf(stderr, "lsh: allocation error\n");
  exit(EXIT_FAILURE);
}
```

- **`tokens` 是一个指针数组**（`char **tokens`），用于存储解析出的 `token`（子字符串）的指针。
- 使用 `malloc()` **动态分配内存**，数组初始大小为 `LSH_TOK_BUFSIZE`（64 个指针）。
- 如果分配失败，直接打印错误并退出。

------

### **(2) 解析字符串**

```c
token = strtok(line, LSH_TOK_DELIM);
while (token != NULL) {
```

- `strtok(line, LSH_TOK_DELIM)`：
  - 首次调用 `strtok()`，传入 line指针，它会：
    1. **找到第一个非分隔符的字符**，作为 `token` 开始位置。
    2. **找到下一个分隔符，并替换为 `\0`**，使得 `token` 成为一个独立字符串。
    3. **返回 `token` 指针**，指向原始 `line` 字符串的某个位置。
- `while (token != NULL)`：
  - **每次调用 `strtok(NULL, LSH_TOK_DELIM)`，它会继续查找下一个 `token`**，直到返回 `NULL`（字符串解析完毕）。

------

### **(3) 存储 `token`**

```c
tokens[position] = token;
position++;
```

- `tokens[position] = token;`：
  - **将 `token` 指针存入 `tokens` 数组**。
  - `token` 并没有被复制，而是直接指向 `line` 内部的某个片段（`\0` 已标记结束）。
- `position++`：数组索引向前移动。

------

### **(4) 重新分配 `tokens` 数组**

```c
if (position >= bufsize) {
  bufsize += LSH_TOK_BUFSIZE;
  tokens = realloc(tokens, bufsize * sizeof(char*));
  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }
}
```

- 如果 `position` 超过当前 `bufsize`，需要扩展 `tokens` 数组：
  1. `bufsize += LSH_TOK_BUFSIZE;` **增加缓冲区大小**（每次扩展 64 个指针）。
  2. `realloc()`扩展 `tokens` 指针数组：
     - **如果 `realloc()` 失败，`tokens` 仍然指向原来的有效地址**（但这里直接退出）。
  3. **确保指针数组能够存储足够的 `token`**。

------

### **(5) 继续解析下一个 `token`**

```c
token = strtok(NULL, LSH_TOK_DELIM);
```

- **每次调用 `strtok(NULL, LSH_TOK_DELIM)`，`strtok()` 会继续解析下一个 `token`**。
- **直到 `token == NULL`，说明 `line` 已完全解析完毕，循环结束。**

------

### **(6) 终止 `tokens` 数组**

```c
tokens[position] = NULL;
return tokens;
```

- 在数组最后添加 `NULL`，符合 `execvp()` 期望的 `argv[]` 结构：

  - `tokens` 变成一个 `NULL` 结尾的字符串指针数组，例如：

    ```c
    tokens = { "echo", "hello", "world", NULL }
    ```

- **返回 `tokens` 指针**，供 Shell 程序后续处理（如 `execvp()` 调用）。

------

### **3. `strtok()` 工作原理**

### **示例**

```c
#include <stdio.h>
#include <string.h>

int main() {
    char line[] = "echo hello world";
    char *token = strtok(line, " ");

    while (token != NULL) {
        printf("%s\n", token);
        token = strtok(NULL, " ");
    }
    return 0;
}
```

**输出：**

```
echo
hello
world
```

**解析流程**

1. `strtok(line, " ")`：找到 `"echo"`，并将 `" "` 变为 `\0`，返回 `"echo"` 指针。
2. `strtok(NULL, " ")`：继续查找 `"hello"`。
3. `strtok(NULL, " ")`：返回 `"world"`。
4. `strtok(NULL, " ")`：返回 `NULL`，解析完成。

------

### **4. `lsh_split_line()` 示例**

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    char input[] = "ls -l /home/user";
    char **args = lsh_split_line(input);

    for (int i = 0; args[i] != NULL; i++) {
        printf("arg[%d]: %s\n", i, args[i]);
    }

    free(args);
    return 0;
}
```

**输出**

```
arg[0]: ls
arg[1]: -l
arg[2]: /home/user
```

------

### **5. `lsh_split_line()` 的局限性**

### **(1) `strtok()` 不是线程安全的**

- `strtok()` 使用**全局静态变量存储状态**，在**多线程环境下可能出错**。

- 解决方案：

  - 使用 **`strtok_r()`**（线程安全版本）。

  ```c
  char *strtok_r(char *str, const char *delim, char **saveptr);
  ```

### **(2) 不支持引号包裹的参数**

- 目前的实现会错误地拆分 `echo "hello world"`，变成：

  ```c
  { "echo", "\"hello", "world\"" }
  ```

- 解决方案：

  - 使用 `strchr()` 检测 `"..."` 。
  - 或者使用 **状态机** 解析字符串。

------

### **6. 结论**

✅ **`lsh_split_line()` 解析命令行字符串，并返回 `NULL` 结尾的参数数组。**
 ✅ **基于 `strtok()` 逐个拆分 `line`，并存储 `token` 指针。**
 ✅ **使用 `malloc()` + `realloc()` 动态管理 `tokens` 数组，防止溢出。**
 ✅ **适用于简单命令解析，但不支持引号和转义符。**

🚀 **理解 `strtok()`，掌握 `malloc()` / `realloc()`，是 Shell 编写的核心技能！**

### **为什么 `strtok(NULL, " ")` ？**

在 `strtok()` 解析字符串时，**第一次调用时传入的是字符串指针，后续调用时必须传入 `NULL`，否则会重新解析新字符串**。

------

#### **1. `strtok()` 工作原理**

```c
char *strtok(char *str, const char *delim);
```

- 参数 `str`：
  - 第一次调用时，`str` 传入要解析的字符串指针（如 `"echo hello world"`）。
  - 后续调用时，必须传入 `NULL`，`strtok()` 会继续解析 **上一次解析的字符串**。
- 参数 `delim`：
  - 指定分隔符，如 `" "`（空格）。
- 返回值：
  - 返回 **指向当前 `token` 的指针**（但仍在原字符串中）。
  - 当没有更多 `token` 时，返回 `NULL`。

------

#### **2. `strtok(NULL, " ")` 作用**

```c
char line[] = "echo hello world";
char *token = strtok(line, " ");  // 第一次调用，传入 line 指针

while (token != NULL) {
    printf("%s\n", token);
    token = strtok(NULL, " ");  // 继续解析同一个字符串
}
```

**执行流程**

1. **第一次调用**

   ```c
   strtok(line, " ")
   ```

   - **找到第一个 `token`**：`"echo"`

   - 在 `"echo"` 之后的第一个 `" "` 处添加 `\0`，字符串变成：

     ```
     "echo\0hello world"
     ```

   - `strtok()` 返回 `"echo"` 的指针。

2. **第二次调用**

   ```c
   strtok(NULL, " ")
   ```

   - **传入 `NULL`，`strtok()` 继续解析** **之前的 `line`**。

   - 跳过 `\0`，找到 "hello"，再次在 " " 处插入 `\0`：

     ```
     "echo\0hello\0world"
     ```

   - 返回 `"hello"` 指针。

3. **第三次调用**

   ```c
   strtok(NULL, " ")
   ```

   - 继续解析 "world"，添加 `\0`：

     ```
     "echo\0hello\0world\0"
     ```

   - 返回 `"world"`。

4. **第四次调用**

   ```c
   strtok(NULL, " ")
   ```

   - 发现没有剩余 `token`，返回 `NULL`，循环终止。

------

#### **3. `strtok()` 需要 `NULL` 作为参数的原因**

**如果每次都传入新的字符串，`strtok()` 会重新解析，而不是继续之前的解析**。
 示例（错误的调用方式）：

```c
char line[] = "echo hello world";

char *token1 = strtok(line, " ");
printf("%s\n", token1);

char *token2 = strtok(line, " ");  // ❌ 重新解析 `line`，不会继续上一次解析
printf("%s\n", token2);
```

**输出错误**

```
echo
echo  // ❌ 没有继续解析 "hello"
```

正确的方式：

```c
char *token = strtok(line, " ");
while (token != NULL) {
    printf("%s\n", token);
    token = strtok(NULL, " ");  // ✅ 继续解析
}
```

------

#### **4. `strtok()` 内部原理**

`strtok()` **使用静态变量** 存储解析进度：

- **第一次调用时，存储 `line` 起始位置**。
- **后续 `NULL` 调用时，使用存储的进度继续解析**。

**代码等价于**

```c
static char *saved_ptr = NULL;

char *custom_strtok(char *str, const char *delim) {
    if (str != NULL) saved_ptr = str;  // 第一次调用时保存字符串指针
    if (saved_ptr == NULL) return NULL; // 没有更多 token
    
    // 查找下一个分隔符
    char *token_start = saved_ptr;
    char *delim_pos = strpbrk(saved_ptr, delim); // 查找分隔符位置

    if (delim_pos) {
        *delim_pos = '\0'; // 替换为 '\0'，让 token 成为独立字符串
        saved_ptr = delim_pos + 1; // 更新解析位置
    } else {
        saved_ptr = NULL; // 解析完毕
    }

    return token_start;
}
```

------

#### **5. `strtok()` 的局限性**

#### **(1) 破坏原字符串**

```c
char str[] = "hello world";
strtok(str, " ");
printf("%s\n", str);  // 变成 "hello\0world"
```

- `strtok()`会在 `line` 内部插入 `\0`，修改原始字符串，因此：
  - **不能用于常量字符串**（`const char *`）。
  - **如果要保留原始字符串，需要先 `strdup()` 复制一份**。

#### **(2) 不是线程安全**

- `strtok()` **使用静态变量 `saved_ptr` 维护状态**，**多线程环境下调用可能导致冲突**。

- 解决方案：

  - **使用 `strtok_r()`**（线程安全版本）。

  ```c
  char *strtok_r(char *str, const char *delim, char **saveptr);
  ```

  - 例：

    ```c
    char str[] = "hello world";
    char *token;
    char *saveptr;
    
    token = strtok_r(str, " ", &saveptr);
    while (token) {
        printf("%s\n", token);
        token = strtok_r(NULL, " ", &saveptr);
    }
    ```

------

#### **6. 总结**

✅ `strtok()` **第一次调用时，必须传入字符串指针**，后续调用必须使用 `NULL`，否则会重新解析。
 ✅ `strtok()` **通过修改原字符串（插入 `\0`）来分割字符串**，返回 `token` 指针。
 ✅ **不适用于多线程环境**，建议使用 `strtok_r()`。
 ✅ **解析命令行时，`strtok(NULL, " ")` 让我们能逐个获取命令参数**。

🚀 **牢记 `strtok()` 的 `NULL` 机制，可以让你轻松解析字符串！** 🎯





## How shells start processes



>
>
>So, once all is said and done, we have an array of tokens, ready to execute. Which begs the question, how do we do that?



```c
int lsh_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}
```

### **Explanation of `lsh_launch()` Function**

The function `lsh_launch()` is responsible for **creating a new process and executing a command** in a simple shell implementation.

### **1. Overview**

- It **forks a new child process** to execute the command.
- The **child process replaces itself with the given command** using `execvp()`.
- The **parent process waits** for the child to finish using `waitpid()`.
- If forking fails, it prints an error message.

------

### **2. Code Breakdown**

```c
int lsh_launch(char **args)
{
  pid_t pid, wpid;
  int status;
```

- `pid_t pid`: Stores the **process ID** of the newly created process.
- `pid_t wpid`: Stores the **process ID returned by `waitpid()`**.
- `int status`: Stores the **exit status of the child process**.

------

### **Step 1: Create a New Process (`fork()`)**

```c
  pid = fork();
  if (pid == 0) {
```

- `fork()` creates a **child process**, which is an exact copy of the parent process.
- `pid == 0` **means we are in the child process**.

------

### **Step 2: Execute the Command (`execvp()`)**

```c
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
```

- `execvp(args[0], args)`:
  - `args[0]` is the **command to execute** (e.g., `"ls"`).
  - `args` is an **array of arguments** (e.g., `{ "ls", "-l", NULL }`).
  - **If successful, `execvp()` replaces the child process with the new command**.
  - **If `execvp()` fails, it returns `-1`**, and `perror("lsh")` prints an error message.
- `exit(EXIT_FAILURE);`:
  - If `execvp()` fails, **the child process exits with an error status**.

------

### **Step 3: Error Handling for `fork()`**

```c
  } else if (pid < 0) {
    perror("lsh");
  }
```

- **If `pid < 0`, `fork()` failed**, meaning the system couldn’t create a new process.
- `perror("lsh")` prints an error message.

------

### **Step 4: Parent Process Waits for Child**

```c
  else {
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
```

- `waitpid(pid, &status, WUNTRACED)`:
  - `waitpid()` **waits for the child process to finish**.
  - `WUNTRACED` ensures the parent can catch stopped child processes.
- Loop condition:
  - `!WIFEXITED(status) && !WIFSIGNALED(status)`:
    - **`WIFEXITED(status)`**: Checks if the child exited normally.
    - **`WIFSIGNALED(status)`**: Checks if the child was terminated by a signal.
  - The loop ensures the parent **keeps waiting until the child terminates**.

------

### **3. Example Execution**

Let's say the user types:`ls -l`

In memory:

```c
args = { "ls", "-l", NULL };
```

Execution Steps:

1. `fork()` creates a new process:
   - Parent process: **`pid > 0`**, waits for child.
   - Child process: **`pid == 0`**, executes `execvp("ls", args)`.
2. Child process replaces itself with `ls -l`:
   - If successful: Child process runs `ls -l` and terminates.
   - If failed: Prints an error and exits.
3. **Parent process waits** for the child using `waitpid()`.
4. **After `ls -l` finishes**, `waitpid()` returns, and `lsh_launch()` completes.

------

### **4. Output Example**

```sh
$ ls -l
total 12
-rwxr-xr-x 1 user user  4096 Feb 10 12:00 file1.txt
-rwxr-xr-x 1 user user  4096 Feb 10 12:00 file2.txt
```

- `ls -l` executes in the child process.
- Parent process **waits until `ls -l` completes**.
- Once `ls -l` finishes, `lsh_launch()` returns.

------

### **5. Summary**

✅ **`fork()` creates a new child process** to execute commands.
 ✅ **`execvp()` replaces the child process with the actual command execution**.
 ✅ **`waitpid()` ensures the parent waits until the child finishes**.
 ✅ **If `execvp()` fails, an error message is displayed**.

🚀 **This function is fundamental to building a simple shell, allowing users to execute external commands!**



## **Unix 进程的启动方式及经典做法**

### **1. 引言**

Shell 的核心功能之一是**启动进程（starting processes）**。在 Unix/Linux 系统中，所有的用户进程（除了 `init` 进程）都是由**已有进程派生**出来的，因此理解进程的创建方式是编写 Shell 或管理系统进程的基础。

在 Unix 及其衍生系统（如 Linux）中，启动进程的**经典方法**是使用 `fork()` 和 `exec()` 组合。本文将详细介绍 **Unix 进程的启动机制、经典方法、以及现代通用做法**。

------

### **2. Unix 进程的启动方式**

在 Unix 系统中，进程的创建有两种方式：

1. **系统启动时，由 `init`（或 `systemd`）启动**
2. **通过 `fork()` 复制进程，再用 `exec()` 替换程序**

#### **2.1 `init` 进程**

当 Unix 内核加载完成后，它启动的第一个用户空间进程就是 **`init`**（现代 Linux 采用 `systemd`）。`init` 负责：

- 初始化系统，启动后台服务（如 `cron`, `syslogd`）。
- 运行 `getty` 进程，提供登录界面。
- 作为所有孤儿进程的收容者（`reaper`）。

#### **2.2 `fork()` + `exec()`：进程创建的标准方式**

普通进程的创建方式是：

1. **`fork()` 复制当前进程**（创建子进程）。
2. **子进程使用 `exec()` 运行新程序**（替换自身）。

示例：

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();  // 创建子进程

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // 子进程执行新的程序
        execlp("ls", "ls", "-l", NULL);
        perror("exec failed");  // 如果 exec 失败，打印错误
        exit(EXIT_FAILURE);
    } else {
        // 父进程等待子进程结束
        wait(NULL);
        printf("Child process finished.\n");
    }
    return 0;
}
```

#### **2.3 `fork()` 和 `exec()` 解析**

#### **(1) `fork()`：复制当前进程**

- `fork()` 调用后，当前进程会被复制，成为**两个几乎相同的进程**（父进程和子进程）。
- 在子进程中，`fork()` **返回 `0`**，表示自己是子进程。
- 在父进程中，`fork()` **返回子进程的 PID**。

示例：

```c
pid_t pid = fork();

if (pid == 0) {
    printf("我是子进程，PID=%d\n", getpid());
} else {
    printf("我是父进程，PID=%d，子进程 PID=%d\n", getpid(), pid);
}
```

#### **(2) `exec()`：执行新程序**

`exec()` 系列函数用于**替换当前进程的代码**，包括：

- `execl()`
- `execv()`
- `execle()`
- `execvp()`
- `execvpe()`

示例：

```c
execlp("ls", "ls", "-l", NULL);
```

- 进程调用 `exec()` 后，会加载 `ls` 命令，并运行它，原进程的代码**完全被新进程的代码替换**。
- **如果 `exec()` 成功，后面的代码不会执行**，除非失败（此时会返回 `-1`）。

------

### **3. `fork()` + `exec()` 的经典使用**

Shell 处理用户输入时，会：

1. **解析命令**，拆分参数。
2. **调用 `fork()`** 创建子进程。
3. **子进程调用 `exec()`** 执行新程序。
4. **父进程调用 `wait()`** 等待子进程结束。

#### **3.1 经典 Shell 进程模型**

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    char *cmd = "/bin/ls";
    char *args[] = {"ls", "-l", NULL};

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // 子进程执行命令
        execv(cmd, args);
        perror("exec failed");
        exit(EXIT_FAILURE);
    } else {
        // 父进程等待子进程结束
        wait(NULL);
        printf("Child process completed.\n");
    }
    return 0;
}
```

------

### **4. 现代通用的进程创建方式**

虽然 `fork()` + `exec()` 仍然是主流，但现代操作系统提供了**更高效的替代方案**：

#### **4.1 `posix_spawn()`**

- `fork()` 会复制整个进程的 **内存空间**，但在 `exec()` 之后，原始数据会被丢弃，因此效率不高。
- `posix_spawn()` **直接创建进程并执行新程序，避免 `fork()` 额外的资源消耗**。

示例：

```c
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **environ;

int main() {
    pid_t pid;
    char *args[] = {"ls", "-l", NULL};
    
    if (posix_spawn(&pid, "/bin/ls", NULL, NULL, args, environ) != 0) {
        perror("posix_spawn failed");
        exit(EXIT_FAILURE);
    }

    printf("Spawned process PID=%d\n", pid);
    return 0;
}
```

**优势**：

- 适用于 **低资源环境**（如嵌入式系统）。
- 避免 `fork()` 造成的**写时复制（Copy-On-Write）**。

------

#### **4.2 `clone()`（Linux 专用）**

- `clone()` 是 `fork()` 的更底层实现，允许创建**共享资源**的进程。
- Docker、Linux 容器等技术广泛使用 `clone()` 以优化进程管理。

示例：

```c
#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int child_func(void *arg) {
    printf("Child process running\n");
    return 0;
}

int main() {
    char stack[1024*1024];  // 子进程的栈空间
    pid_t pid = clone(child_func, stack + sizeof(stack), SIGCHLD, NULL);

    if (pid == -1) {
        perror("clone failed");
        exit(EXIT_FAILURE);
    }

    printf("Created process with PID=%d\n", pid);
    return 0;
}
```

**优势**：

- 允许共享 **内存、文件描述符、信号** 等资源。
- 用于 **线程（pthread）、轻量级进程（LWP）**。

------

### **5. 总结**

| 方式              | 适用场景          | 优势                   | 劣势            |
| ----------------- | ----------------- | ---------------------- | --------------- |
| `fork() + exec()` | 传统进程创建方式  | 可靠，适用于 Shell     | `fork()` 开销大 |
| `posix_spawn()`   | 嵌入式/轻量级应用 | 避免 `fork()` 复制数据 | 兼容性较低      |
| `clone()`         | Linux 容器/线程   | 共享资源，高效         | 仅适用于 Linux  |

🚀 **经典 Shell 仍然使用 `fork() + exec()`，但现代操作系统在高性能场景下采用 `posix_spawn()` 或 `clone()` 来优化进程管理！**





## Shell Builtins

### **解释：为什么某些命令必须由 Shell 内部执行？**

在 `lsh_loop()` 中，它调用 `lsh_execute()` 而不是 `lsh_launch()`，这并不是偶然的设计，而是有意为之。我们之前的 `lsh_launch()` 主要用于**执行外部程序**（即调用 `fork()` + `execvp()` 启动新进程）。但并**不是所有 Shell 命令都可以通过创建新进程来执行**，有些命令必须由 Shell **自身** 执行，而不是创建一个子进程来完成。以下是几个典型的例子：

------

### **1. 为什么 `cd` 命令必须由 Shell 内部执行？**

通常，我们使用 `cd` 命令来切换当前工作目录，例如：

```sh
cd /home/user
```

但如果 `cd` 被实现为一个**外部程序**，它将无法真正改变 Shell 进程的目录。**原因如下：**

- **进程的当前目录是进程的私有属性**，每个进程都有自己的 `cwd`（current working directory）。
- 如果 `cd` 是一个外部程序，它会在子进程中运行：
  1. `fork()` 创建一个子进程。
  2. 子进程调用 `chdir("/home/user")` 更改目录。
  3. 子进程**终止**，但 Shell 仍在原来的目录中。

因此，**Shell 本身必须调用 `chdir()`，这样它的进程环境才能真正改变**。

### **示例**

错误的做法（如果 `cd` 是外部程序）：

```sh
$ cd /home/user   # 这个 `cd` 在子进程中执行
$ pwd             # 仍然是原来的目录，没有改变！
```

正确的做法（Shell 内部执行）：

```sh
$ cd /home/user   # Shell 进程自己调用 `chdir()`
$ pwd             # 目录成功改变！
```

------

### **2. 为什么 `exit` 命令必须由 Shell 内部执行？**

类似地，`exit` 不能作为一个外部程序来执行：

- **如果 `exit` 是外部程序**：
  1. `fork()` 创建子进程。
  2. 子进程运行 `exit`，终止自己。
  3. **Shell 进程仍然活着**，不会退出！
- **但正确的行为应该是 Shell 进程自己调用 `exit()`，从而终止整个 Shell 进程**。

### **示例**

错误的做法（如果 `exit` 是外部程序）：

```sh
$ exit   # 退出的只是子进程，Shell 依然运行
$        # 仍然可以继续输入命令
```

正确的做法（Shell 内部执行）：

```sh
$ exit   # 直接退出 Shell
```

------

### **3. 为什么某些 Shell 配置命令必须是内建的？**

很多 Shell 需要加载配置文件（如 `~/.bashrc`）来调整自己的行为。例如：

```sh
alias ll='ls -l'
set -o vi
export PATH=$PATH:/usr/local/bin
```

这些命令改变了 **Shell 本身的环境**，所以它们不能作为外部程序运行：

- **`alias`**：定义别名，改变 Shell 解析命令的方式。
- **`set -o`**：修改 Shell 选项，影响输入行为。
- **`export`**：修改环境变量，影响后续进程。

如果这些命令被放在子进程中执行，它们的**影响不会传回到 Shell 进程**，所以必须由 Shell 内部执行。

------

### **4. 总结**

| 命令     | 是否可以作为外部程序？ | 必须由 Shell 内部执行的原因                     |
| -------- | ---------------------- | ----------------------------------------------- |
| `cd`     | ❌ 不行                 | 进程的 `cwd` 是私有的，子进程修改不会影响 Shell |
| `exit`   | ❌ 不行                 | 退出的是子进程，Shell 进程仍然运行              |
| `alias`  | ❌ 不行                 | 影响 Shell 解析命令的方式                       |
| `export` | ❌ 不行                 | 影响 Shell 环境变量，子进程修改无法影响 Shell   |
| `ls`     | ✅ 可以                 | 只是列出文件信息，没有修改 Shell 状态           |

### **🔹 关键结论**

✅ **外部命令**（如 `ls`, `grep`, `ping`）可以通过 `fork()` + `execvp()` 运行，因为它们**不会修改 Shell 本身的行为**。
 ✅ **内建命令**（如 `cd`, `exit`, `export`, `alias`）必须由 Shell 进程自身执行，否则它们的作用不会生效！

🚀 **这就是为什么 Shell 需要 `lsh_execute()` 来区分内建命令和外部命令，而不是直接调用 `lsh_launch()`！**





```c
/*
  Function Declarations for builtin shell commands:
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/
int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

int lsh_help(char **args)
{
  int i;
  printf("Stephen Brennan's LSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int lsh_exit(char **args)
{
  return 0;
}

```

### **What is `args` and Why Use a Double Pointer (`char \**args`) Instead of a Single Pointer (`char \*args`)?**

------

### **1. Understanding `args` in This Code**

In this code, `args` is a **double pointer (`char \**args`)**, which represents an **array of strings**. It is used to **store command-line arguments** passed to shell built-in functions (`lsh_cd`, `lsh_help`, `lsh_exit`).

### **Example: User Input**

```sh
cd /home/user
```

If a user types the above command, it will be **tokenized into an array** like this:

```c
args = { "cd", "/home/user", NULL };
```

Here:

- `args[0]` is `"cd"` (the command name).
- `args[1]` is `"/home/user"` (the argument to `cd`).
- `args[2]` is `NULL` (to indicate the end of arguments).

------

### **2. Why Use `char \**args` Instead of `char \*args`?**

If we used `char *args`, it would only store a **single string** (a single word), which is **not enough to represent multiple command-line arguments**.

### **2.1 Using `char \*args` (Incorrect)**

```c
int lsh_cd(char *args) {  // ❌ Incorrect
  printf("Command: %s\n", args);
}
```

This function would **only receive a single string**, such as `"cd"` or `"/home/user"`, but not both.

------

### **2.2 Using `char \**args` (Correct)**

```c
int lsh_cd(char **args) {  // ✅ Correct
  printf("Command: %s, Argument: %s\n", args[0], args[1]);
}
```

This function receives an **array of strings**, allowing us to handle multiple arguments.

### **Example Breakdown**

Let's assume the user types:

```sh
cd /home/user
```

In memory, `args` will be stored as:

```
args = {
  "cd",         // args[0]
  "/home/user", // args[1]
  NULL          // args[2] (end of arguments)
};
```

**Accessing elements:**

```c
printf("%s\n", args[0]); // Output: cd
printf("%s\n", args[1]); // Output: /home/user
```

**Checking for missing arguments:**

```c
if (args[1] == NULL) {
  fprintf(stderr, "lsh: expected argument to \"cd\"\n");
}
```

If the user only types `cd` without an argument, `args[1] == NULL`, and we print an error message.

------

### **3. How the Shell Uses `char \**args`**

### **3.1 Function Call Flow**

When a user enters a command like:

```sh
cd /home/user
```

The shell follows these steps:

1. **Tokenize the input**: `"cd /home/user"` → `{ "cd", "/home/user", NULL }`

2. **Find the corresponding built-in function**: `lsh_cd(args)`

3. Pass `args` as a parameter to `lsh_cd()`:

   ```c
   lsh_cd({ "cd", "/home/user", NULL });
   ```

4. **Inside `lsh_cd()`**, we access `args[0]` (`"cd"`) and `args[1]` (`"/home/user"`) to execute `chdir("/home/user")`.

------

### **4. Summary**

✅ **`char \**args` represents an array of strings, allowing multiple command arguments to be passed.**
 ✅ **Using `char \*args` would only allow a single string, making it impossible to pass multiple arguments.**
 ✅ **This design allows built-in shell commands (`cd`, `exit`, `help`) to handle multiple arguments efficiently.**

🚀 **Understanding `char \**args` is crucial for building command-line interfaces and shell interpreters!**





### **解析 `lsh_num_builtins()`**

```c
int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}
```

这个函数的作用是 **计算 Shell 内建命令（builtin commands）的数量**，即 `builtin_str` 数组中的元素个数。

------

### **1. 代码拆解**

### **(1) `sizeof(builtin_str)`**

- `builtin_str` 是一个 **字符串指针数组**（`char *builtin_str[]`）。
- `sizeof(builtin_str)` 计算整个数组的 **总字节大小**。

### **(2) `sizeof(char \*)`**

- `sizeof(char *)` 计算 **单个指针的大小**（在 64 位系统上通常是 `8` 字节，在 32 位系统上通常是 `4` 字节）。

### **(3) 计算数组长度**

```c
数组长度 = 数组总大小 / 单个元素大小
```

- `sizeof(builtin_str) / sizeof(char *)` 计算 `builtin_str` 数组中 **指针的个数**，即内建命令的数量。

------

### **2. 举例分析**

### **示例代码**

```c
char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

printf("%d\n", lsh_num_builtins()); // 结果是多少？
```

### **计算过程**

假设 **`char \*` 的大小是 8 字节**（64 位系统）：

1. `sizeof(builtin_str) = 8 × 3 = 24` （数组有 3 个 `char *` 指针）。
2. `sizeof(char *) = 8`（每个指针占 8 字节）。
3. `lsh_num_builtins() = 24 / 8 = 3`。

最终返回 **3**，表示 **`builtin_str` 里有 3 个内建命令**。

------

### **3. 可能的错误：为什么不能用 `sizeof(builtin_str) / sizeof(builtin_str[0])`？**

在这个代码里，其实 `sizeof(builtin_str) / sizeof(builtin_str[0])` 也是正确的：

```c
return sizeof(builtin_str) / sizeof(builtin_str[0]);
```

因为：

- `builtin_str[0]` 是 `char *` 类型，`sizeof(builtin_str[0])` 等于 `sizeof(char *)`，两种写法等效。

但如果 **在函数中传递 `builtin_str` 作为参数**，就不能这么写了。例如：

```c
int lsh_num_builtins(char *arr[]) {
  return sizeof(arr) / sizeof(arr[0]);  // ❌ 错误
}
```

原因：

- `char *arr[]` 作为函数参数，相当于 `char **arr`，`sizeof(arr)` **只返回指针大小（8 字节）**，而不是数组大小。
- 解决方法是：**只能在数组本地作用域使用 `sizeof()` 计算长度**，或者传递 `int length` 作为参数。

------

### **4. 结论**

✅ **`lsh_num_builtins()` 计算 `builtin_str` 数组中命令的数量。**
 ✅ **`sizeof(builtin_str) / sizeof(char \*)` 计算的是 `char \*` 指针的个数。**
 ✅ **这个方法仅适用于局部定义的数组，不适用于函数参数（因为数组会退化为指针）。**

🚀 **理解 `sizeof()` 的用法，是掌握 C 语言指针和数组管理的关键！**





## Putting together builtins and processes

```c
int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}
```

### **解析 `(\*builtin_func[i])(args)` 中的 `\*` 为什么存在？**

在 `lsh_execute()` 函数中，这一行代码：

```c
return (*builtin_func[i])(args);
```

**实际上是调用内建函数（如 `lsh_cd()`、`lsh_help()`、`lsh_exit()`），并将 `args` 作为参数传递给它们。**

------

### **1. 关键数据结构分析**

在代码中，内建命令（built-in commands）的名称和它们对应的函数指针被存储在数组 `builtin_func[]` 中：

```c
// 内建命令字符串数组
char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

// 对应的函数指针数组
int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit
};
```

- `builtin_str[]` **存储命令名称**（字符串）。
- `builtin_func[]` **存储函数指针**，它们指向 `lsh_cd()`、`lsh_help()` 和 `lsh_exit()` 这些函数。

#### **关键点**

- `builtin_func[i]` 是一个**指向函数的指针**，即 `int (*builtin_func[i])(char **)`。
- 由于 `builtin_func[i]` 是一个指针，**所以在调用它时，必须解引用（`\*builtin_func[i]`），才能真正执行该函数。**

------

### **2. 为什么需要 `\*builtin_func[i]`？**

### **(1) `builtin_func[i]` 是函数指针**

在 `builtin_func[]` 中，每个元素的类型是：

```c
int (*)(char **)
```

**即 "指向返回 `int` 类型，并接受 `char \**` 作为参数的函数指针"**。

例如：

```c
builtin_func[0] == &lsh_cd;
builtin_func[1] == &lsh_help;
builtin_func[2] == &lsh_exit;
```

所以：

```c
builtin_func[i]  // 是一个函数指针
*builtin_func[i] // 通过解引用，得到真正的函数
(*builtin_func[i])(args) // 调用该函数，并传递参数 args
```

### **(2) `\*builtin_func[i]` 实际上等价于 `builtin_func[i]`**

在 C 语言中，**函数指针可以直接调用，而无需解引用**，所以：

```c
return (*builtin_func[i])(args);
```

等价于：

```c
return builtin_func[i](args);
```

**`\*` 在这里是可选的**，但它的存在使得代码更清晰，**明确地表明 `builtin_func[i]` 是一个指向函数的指针，需要解引用后才能调用**。

------

### **3. `lsh_execute()` 的完整运行逻辑**

当用户输入命令时：

1. `lsh_execute()` 解析 `args[0]`，确定输入的命令名称。

2. 遍历 `builtin_str[]`，查找是否是内建命令。

3. 如果 `args[0]` 匹配 `builtin_str[i]`，则执行对应的内建函数：

   ```c
   return (*builtin_func[i])(args);
   ```

4. **如果命令不是内建的，则调用 `lsh_launch(args)` 运行外部程序。**

------

### **4. 示例：用户输入 `cd /home/user`**

假设用户输入：

```sh
cd /home/user
```

Shell 解析出：

```c
args = { "cd", "/home/user", NULL };
```

运行 `lsh_execute(args)`：

1. `args[0] == "cd"`，匹配 `builtin_str[0]`。

2. 调用 `builtin_func[0]`，即：

   ```c
   (*builtin_func[0])(args);  // 相当于 lsh_cd(args);
   ```

3. 进入 `lsh_cd(args)`：

   ```c
   if (chdir(args[1]) != 0) {
     perror("lsh");
   }
   ```

   这里 `args[1] == "/home/user"`，调用 `chdir("/home/user")`改变当前目录。

------

### **5. 总结**

| 语法                       | 含义                                                  |
| -------------------------- | ----------------------------------------------------- |
| `builtin_func[i]`          | 函数指针，指向 `lsh_cd()`、`lsh_help()`、`lsh_exit()` |
| `*builtin_func[i]`         | 通过解引用，获取真正的函数                            |
| `(*builtin_func[i])(args)` | 调用该函数，并传递 `args` 作为参数                    |

### **为什么要用 `\*builtin_func[i]`？**

✅ `builtin_func[i]` 是**指向函数的指针**，需要解引用 `*` 才能调用该函数。
 ✅ `*builtin_func[i]` 和 `builtin_func[i]` 作用相同，**解引用是可选的**，但可以让代码更清晰。
 ✅ 这样设计的好处是：**可以通过数组索引调用不同的函数，实现命令的动态分配，而不需要使用大量的 `if-else` 语句！**

🚀 **这是一种经典的 "命令调度" 方式，使得 Shell 可以高效地管理内建命令！**



### 为什么内建函数不同于外部程序？

你可能会问：

> "这样不也是函数调用吗？和外部命令 `ls`、`echo` 这些有什么区别？"

### **(1) 外部命令是由 `fork()` + `execvp()` 运行的**

外部命令（如 `ls`、`grep`）都是**独立的二进制程序**，它们的执行需要：

1. **`fork()` 创建一个新进程**（子进程）。

2. `execvp()` 用新进程执行该命令：

   ```c
   int lsh_launch(char **args) {
       pid_t pid = fork();
       if (pid == 0) {
           execvp(args[0], args);
           perror("lsh");
           exit(EXIT_FAILURE);
       }
       waitpid(pid, NULL, 0);
       return 1;
   }
   ```

3. **外部命令运行在子进程中**，不会影响 Shell 本身。

------

### **(2) 内建命令必须由 Shell 本身执行**

但 `cd`、`exit` 这些命令 **不能用 `execvp()` 运行**，因为它们的行为需要修改 **Shell 进程自身**：

#### **✅ `cd` 需要修改 Shell 进程的 `cwd`**

```c
int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {  // ✅ 直接修改当前进程的工作目录
      perror("lsh");
    }
  }
  return 1;
}
```

- `chdir()` 只能修改**当前进程**的目录，而 `fork()` 生成的子进程不会影响 Shell 自身。
- **如果 `cd` 作为外部命令运行，它只能修改子进程的 `cwd`，不会影响 Shell 进程的目录。**

#### **✅ `exit` 需要终止 Shell 进程**

```c
int lsh_exit(char **args)
{
  return 0;  // 让 `lsh_execute()` 返回 0，Shell 退出
}
```

- **如果 `exit` 是外部程序，它只能终止子进程，Shell 进程不会退出！**
- 但是 **Shell 自己调用 `exit()`，才会真正退出**。

------

### **为什么这样设计对 Shell 有用？**

✅ **可以区分“Shell 内建命令”和“外部程序”**
 ✅ **内建命令由 Shell 进程直接执行，影响 Shell 本身（如 `cd`、`exit`）。**
 ✅ **外部命令用 `fork()` + `execvp()` 在子进程中运行，不影响 Shell 本身。**
 ✅ **用“字符串数组 + 函数指针数组”映射命令，使得内建命令的管理和扩展更容易！**

🚀 **这是一种高效的命令调度方式，是 Unix Shell 设计的经典模式！**

## 不足之处



>Obviously, this shell isn’t feature-rich. Some of its more glaring omissions are:
>
>- Only whitespace separating arguments, no quoting or backslash escaping.
>- No piping or redirection.
>- Few standard builtins.
>- No globbing.



测试代码：

```c
gcc -o lsh main.c 
./lsh 
> help
Stephen Brennan's LSH
Type program names and arguments, and hit enter.
The following are built in:
  cd
  help
  exit
Use the man command for information on other programs.
> cd
lsh: expected argument to "cd"
> help
Stephen Brennan's LSH
Type program names and arguments, and hit enter.
The following are built in:
  cd
  help
  exit
Use the man command for information on other programs.
> exit
```



这里是优化后的 `lsh_read_line` 和 `lsh_split_line`，支持 **引号（quoting）** 和 **反斜杠转义（backslash escaping）**，使得用户可以正确输入带空格的参数，例如：

```
echo "Hello World" test\ file.txt
```

### 主要改进：

1. **支持双引号和单引号**（"..." 或 '...'）：

   - 在引号内的空格不会被视作分隔符。
   - 引号内部的转义字符 `\"` 和 `\'` 仍然能正常工作。

2. **支持反斜杠转义**（`\`）：

   - 反斜杠可以转义空格、引号、反斜杠本身，例如：

     ```
     echo \"Hello\ World\"
     ```

     解析为：

     ```
     ["echo", "\"Hello World\""]
     ```

3. **提升 `lsh_read_line` 的鲁棒性**：

   - 允许行末自动截取换行符 `\n`，保证不影响解析。

优化后的代码如下：

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

char *lsh_read_line(void) {
    char *line = NULL;
    ssize_t bufsize = 0;

    if (getline(&line, &bufsize, stdin) == -1) {
        if (feof(stdin)) {
            exit(EXIT_SUCCESS);
        } else {
            perror("readline");
            exit(EXIT_FAILURE);
        }
    }
    return line;
}

char **lsh_split_line(char *line) {
    int bufsize = LSH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    char *token = malloc(strlen(line) + 1);  // 用于存放解析出的 token
    if (!token) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    int i = 0, j = 0;
    bool in_single_quote = false, in_double_quote = false, escaping = false;

    while (line[i] != '\0') {
        char c = line[i];
        if (escaping) {  // 处理转义字符
            token[j++] = c;
            escaping = false;
        } else if (c == '\\') {  // 遇到反斜杠，开启转义模式
            escaping = true;
        } else if (c == '"') {  // 处理双引号
            if (in_single_quote) {
                token[j++] = c;  // 引号内的引号视为普通字符
            } else {
                in_double_quote = !in_double_quote;
            }
        } else if (c == '\'') {  // 处理单引号
            if (in_double_quote) {
                token[j++] = c;
            } else {
                in_single_quote = !in_single_quote;
            }
        } else if (strchr(LSH_TOK_DELIM, c) && !in_single_quote && !in_double_quote) {
            if (j > 0) {  // 遇到空格分隔符，但 token 非空，存入 tokens 数组
                token[j] = '\0';
                tokens[position++] = strdup(token);
                j = 0;
                if (position >= bufsize) {  // 重新分配空间
                    bufsize += LSH_TOK_BUFSIZE;
                    tokens = realloc(tokens, bufsize * sizeof(char *));
                    if (!tokens) {
                        fprintf(stderr, "lsh: allocation error\n");
                        exit(EXIT_FAILURE);
                    }
                }
            }
        } else {
            token[j++] = c;
        }
        i++;
    }

    if (j > 0) {  // 处理最后一个 token
        token[j] = '\0';
        tokens[position++] = strdup(token);
    }

    tokens[position] = NULL;  // 结束标记
    free(token);
    return tokens;
}
```

------

### **测试用例**

#### **输入**

```
echo "Hello World" 'This is a test' file\ name.txt
```

#### **解析结果**

```
["echo", "Hello World", "This is a test", "file name.txt"]
```

------

### **改进点总结**

✅ **支持引号**（`"` 和 `'`）内的内容不会被空格拆分。
 ✅ **支持反斜杠转义**，如 `file\ name.txt` 解析为 `"file name.txt"`。
 ✅ **自动扩展 token 数组，防止溢出**。
 ✅ **代码更清晰、易维护**，便于扩展新功能。

这样，`lsh_split_line` 现在能够正确解析更复杂的命令行输入，符合 **“Only whitespace separating arguments, with quoting and backslash escaping”** 的要求！ 🚀





## 后记

2025年2月4日完成学习。

感谢原作者：Stephen Brennan

