

# Tutorial - Write a Shell in C



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