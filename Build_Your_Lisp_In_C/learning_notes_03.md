

# Build Your Own Lisp

Learn C and build your own programming language in 1000 lines of code!

Link: [https://www.buildyourownlisp.com/](https://www.buildyourownlisp.com/)

Link: [https://www.buildyourownlisp.com/contents](https://www.buildyourownlisp.com/contents)

Source code: [https://github.com/orangeduck/BuildYourOwnLisp/tree/master/src](https://github.com/orangeduck/BuildYourOwnLisp/tree/master/src)



# Variables • Chapter 11

### **正则表达式的解析**

#### **正则表达式**：

```regex
/[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/
```

这是一段用于匹配符号（symbol）的正则表达式，能够匹配一组字符的组合，涵盖标识符、运算符和其他常用符号。下面分部分解析：

------

### **1. `/[a-zA-Z0-9_+\\-\*\\/\\\\=<>!&]+/` 的结构**

- `/`：正则表达式的开始和结束标志，用于包裹正则表达式内容。
- `[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]`：
  - **`[]`**：表示字符范围（range specifier），匹配括号内任意一个字符。
  - **`a-zA-Z`**：匹配小写和大写字母。
  - **`0-9`**：匹配数字。
  - **`_`**：匹配下划线。
  - **`+`、`-`、`\*`、`/`**：匹配算术运算符。
  - **`\\`**：匹配反斜杠（`\`），需要特别解释（见下文）。
  - **`=`、`<`、`>`、`!`**：匹配比较运算符。
  - **`&`**：匹配逻辑与符号。
- `+`：
  - 表示匹配 **一个或多个** `[ ]` 中定义的字符。

------

### **2. `\\` 在正则表达式中的含义**

在正则表达式中，反斜杠（`\`）是一个特殊字符，用于转义其他字符，使其具有特殊意义或保留原意。

#### **为什么需要 `\\\\`？**

这是因为：

1. **C 字符串中的转义规则**：
   - 在 C 字符串中，反斜杠本身是一个特殊字符，用于转义其他字符，例如 `\n` 表示换行。
   - 如果你想在字符串中表示一个普通的反斜杠，需要用两个反斜杠 `\\`。
2. **正则表达式中的转义规则**：
   - 在正则表达式中，`\` 是转义字符。
   - 为了匹配一个普通的反斜杠 `\`，需要在正则表达式中写成 `\\`。
3. **结合 C 和正则的双重转义**：
   - 在 C 中编写正则表达式时，C 字符串的 `\\` 会被解释为一个单反斜杠 `\`。
   - 然后正则表达式中的 `\\` 表示匹配普通反斜杠。

因此，正则表达式中的 `\\\\`：

- 在 C 中表示一个正则表达式的 `\\`，最终用于匹配输入中的一个反斜杠 `\`。

------

### **3. 其他部分的详细解析**

#### **`a-zA-Z`**

- 匹配所有英文字母（大小写）。

#### **`0-9`**

- 匹配所有数字。

#### **`_`**

- 匹配下划线，常用于标识符。

#### **`+\\-\*`**

- 匹配加号（`+`）、减号（`-`）、星号（`*`）。
- 星号需要转义，因为它在正则中是量词。

#### **`\\/`**

- 匹配斜杠 `/`，斜杠需要转义。

#### **`\\\\`**

- 匹配反斜杠 `\`，双重转义的结果。

#### **`=<>!`**

- 匹配赋值和比较运算符（`=`、`<`、`>`、`!`）。

#### **`&`**

- 匹配逻辑与符号 `&`。

------

### **4. 匹配示例**

根据这个正则表达式，以下符号都是合法匹配的：

- 普通标识符：`abc123`、`_varName`
- 算术符号：`+`, `-`, `*`, `/`
- 比较符号：`<=`, `>=`, `==`
- 包含反斜杠：`a\b\c`
- 包含逻辑运算符：`&` 或 `&&`

------

### **总结**

- `\\\\`：
  - 在 C 中表示一个反斜杠。
  - 在正则表达式中用于匹配输入中的普通反斜杠 `\`。
- 正则表达式整体功能：
  - 匹配包含英文字母、数字、下划线、算术运算符、比较运算符、反斜杠和逻辑运算符的符号。
- 用途：
  - 提供足够灵活的规则，允许定义复杂的符号（symbols）。



解释正则表达式

```c
mpca_lang(MPCA_LANG_DEFAULT,
  "                                                     \
    number : /-?[0-9]+/ ;                               \
    symbol : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;         \
    sexpr  : '(' <expr>* ')' ;                          \
    qexpr  : '{' <expr>* '}' ;                          \
    expr   : <number> | <symbol> | <sexpr> | <qexpr> ;  \
    lispy  : /^/ <expr>* /$/ ;                          \
  ",
  Number, Symbol, Sexpr, Qexpr, Expr, Lispy);
```

### **`lispy : /^/ <expr>\* /$/ ;` 的含义**

这是一条语法规则，定义了语言 `lispy` 的整体结构。它描述了合法的 Lispy 表达式必须符合以下要求：

1. **`/^/`**：匹配输入的开头（start of input）。
2. **`<expr>\*`**：匹配零个或多个 `<expr>`（表达式）。
3. **`/$/`**：匹配输入的结尾（end of input）。

通过这个规则，`lispy` 表示的是一组由表达式组成的合法输入，它可以是空的，也可以包含多个表达式，必须覆盖整个输入字符串。

------

### **`/^/` 和 `/$/` 的作用**

- **`/^/`**：匹配字符串的起始位置。
  - 确保整个输入必须从头开始匹配。
  - 如果输入中有多余的前缀内容，匹配会失败。
- **`/$/`**：匹配字符串的结束位置。
  - 确保整个输入必须到结尾完全匹配。
  - 如果输入中有多余的后缀内容，匹配会失败。

这两个标志结合起来，确保整个输入字符串都符合语法规则，而不仅仅是部分内容。

------

### **举例说明**

假设 `<expr>` 是一组可以匹配的表达式，例如数字、符号、S-Expression 或 Q-Expression。

#### **合法的输入示例**

1. **单个表达式**：

   ```text
   123
   ```

   - `123` 是一个 `<number>`，符合 `<expr>`。
   - 整个字符串从开头到结尾都被匹配。

2. **多个表达式**：

   ```text
   123 (+ 1 2) {3 4}
   ```

   - `123` 是 `<number>`。
   - `(+ 1 2)` 是 `<sexpr>`。
   - `{3 4}` 是 `<qexpr>`。
   - 所有的内容从开头到结尾都是合法的表达式。

3. **空输入**：

   ```text
   
   ```

   - 空输入符合 `<expr>*`，因为 `*` 允许匹配零个表达式。

------

#### **非法的输入示例**

1. **前面有多余内容**：

   ```text
   foo 123
   ```

   - `foo` 是未定义的内容，不属于 `<expr>`。
   - 由于 `/^/` 要求从输入的起始位置开始匹配，匹配失败。

2. **后面有多余内容**：

   ```text
   123 bar
   ```

   - `123` 是合法的 `<number>`。
   - `bar` 是未定义的内容，不属于 `<expr>`。
   - 由于 `/$/` 要求匹配到输入的结束，匹配失败。

3. **中间有非法内容**：

   ```text
   123 foo (+ 1 2)
   ```

   - `123` 和 `(+ 1 2)` 是合法的 `<expr>`。
   - `foo` 是非法的内容，导致匹配失败。

------

### **总结**

- **`/^/` 和 `/$/`** 确保整个输入字符串从开头到结尾都被匹配，而不是仅匹配其中的某一部分。
- 合法示例：
  - `123`
  - `(+ 1 2)`
  - `{1 2 3}`
  - `123 (+ 1 2) {3 4}`
- 非法示例：
  - 前后或中间有未定义的内容，如 `foo 123`、`123 bar`。

这两个标志使得解析器能够严格验证输入是否符合 Lispy 的语法规则。



对于函数指针的学习

```c
typedef lval*(*lbuiltin)(lenv*, lval*);
```



### **Function Pointers in C**

Function pointers are one of the most powerful features of C, allowing you to store and pass pointers to functions. This feature enables flexible and dynamic program designs, such as callbacks, dynamic function dispatch, and polymorphism-like behaviors.

------

### **1. What is a Function Pointer?**

A function pointer is a pointer type specifically designed to point to a function’s address. With function pointers, you can call the pointed function as if it were a normal function.

#### **Key Characteristics:**

1. **Stores Function Address**: A function pointer holds the memory address of a function.
2. **Calls Functions Dynamically**: You can invoke the function pointed to by the function pointer.
3. **Type Matching**: The function pointer's type must match the type of the function it points to (return type and parameter types).

------

### **2. Syntax of Function Pointers**

#### **1. Declaring a Function Pointer**

The syntax for declaring a function pointer might seem tricky at first:

```c
return_type (*pointer_name)(parameter_list);
```

For example:

```c
int (*func_ptr)(int, int);
```

- **`int`**: The return type of the function.
- **`(\*func_ptr)`**: Declares a pointer to a function.
- **`(int, int)`**: Specifies the function’s parameter types.

#### **2. Assigning a Function Pointer**

You can assign the address of a function to the pointer:

```c
int add(int a, int b) {
    return a + b;
}

int (*func_ptr)(int, int) = add; // Assign function address to the pointer
```

#### **3. Calling a Function Pointer**

To call the function, use the function pointer like a regular function:

```c
int result = func_ptr(2, 3); // Calls add(2, 3)
```

------

### **3. Use Cases for Function Pointers**

#### **1. Callback Functions**

Function pointers are widely used to implement callbacks, where one function is passed as an argument to another.

**Example: Customizable Sorting**

```c
#include <stdio.h>

int compare_asc(int a, int b) {
    return a - b;
}

int compare_desc(int a, int b) {
    return b - a;
}

void sort(int* array, int size, int (*cmp)(int, int)) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (cmp(array[j], array[j + 1]) > 0) {
                int temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

int main() {
    int array[] = {5, 2, 9, 1, 5, 6};
    int size = sizeof(array) / sizeof(array[0]);

    sort(array, size, compare_asc); // Sort in ascending order
    for (int i = 0; i < size; i++) printf("%d ", array[i]);
    printf("\n");

    sort(array, size, compare_desc); // Sort in descending order
    for (int i = 0; i < size; i++) printf("%d ", array[i]);
    return 0;
}
```

#### **2. Dynamic Function Calls**

Function pointers allow you to dynamically choose and call functions at runtime.

**Example: Choosing Operations Dynamically**

```c
#include <stdio.h>

int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

int main() {
    int (*operation)(int, int);

    int choice = 1; // 1 for add, 2 for subtract
    if (choice == 1) {
        operation = add;
    } else {
        operation = subtract;
    }

    printf("Result: %d\n", operation(10, 5)); // Calls add(10, 5)
    return 0;
}
```

#### **3. Simulating Polymorphism**

By storing pointers to different functions in an array, you can simulate polymorphism.

**Example: Array of Function Pointers**

```c
#include <stdio.h>

int add(int a, int b) { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }

int main() {
    int (*operations[])(int, int) = {add, subtract, multiply};
    
    printf("Add: %d\n", operations[0](4, 2));       // Calls add(4, 2)
    printf("Subtract: %d\n", operations[1](4, 2)); // Calls subtract(4, 2)
    printf("Multiply: %d\n", operations[2](4, 2)); // Calls multiply(4, 2)

    return 0;
}
```

------

### **4. Advanced Use Cases**

#### **1. Function Pointer to a Function Returning a Pointer**

If a function returns a pointer, the syntax becomes more complex:

```c
int* (*func_ptr)(int);
```

- This means `func_ptr` is a pointer to a function that takes an `int` and returns an `int*`.

#### **2. Simplifying with `typedef`**

Complex function pointer declarations can be simplified using `typedef`:

```c
typedef int (*operation)(int, int);

int add(int a, int b) {
    return a + b;
}

int main() {
    operation op = add; // Simplified syntax
    printf("Add: %d\n", op(5, 3));
    return 0;
}
```

------

### **5. Key Considerations and Pitfalls**

1. **Type Matching**:
   - A function pointer must have the same signature (return type and parameters) as the function it points to.
2. **Debugging**:
   - Since function pointers rely on runtime behavior, debugging dynamic calls can be challenging.
3. **Complex Syntax**:
   - The syntax for declaring and using function pointers can be confusing, especially with nested or complex types.

------

### **6. Summary**

1. **Definition**: A function pointer stores the address of a function, allowing dynamic function calls.
2. **Syntax**: While initially complex, syntax can be understood by breaking it down into parts.
3. Applications:
   - Callbacks
   - Dynamic function dispatch
   - Polymorphism-like behaviors
4. **Simplification**: Use `typedef` to make complex function pointers easier to manage.

Function pointers are an essential feature of C, enabling powerful programming techniques and giving developers control over runtime behavior. Mastering them can significantly improve your understanding of C's flexibility and power.