# Build Your Own Lisp

Learn C and build your own programming language in 1000 lines of code!

Link: [https://www.buildyourownlisp.com/](https://www.buildyourownlisp.com/)

Link: [https://www.buildyourownlisp.com/contents](https://www.buildyourownlisp.com/contents)

## Introduction • Chapter 1



## Installation • Chapter 2



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



## Basics • Chapter 3



## An Interactive Prompt • Chapter 4



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



## Languages • Chapter 5

