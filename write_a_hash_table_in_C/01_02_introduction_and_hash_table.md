## Reference

本文是学习这个项目的笔记：[https://github.com/jamesroutley/write-a-hash-table](https://github.com/jamesroutley/write-a-hash-table)



## Hash table structure

Our key-value pairs (items) will each be stored in a `struct`，

Our hash table stores an array of pointers to items, and some details about its size and how full it is:

```c
// hash_table.h
typedef struct {
    char* key;
    char* value;
} ht_item;

// hash_table.h
typedef struct {
    int size;
    int count;
    ht_item** items;
} ht_hash_table;
```

这段C语言代码定义了一个简单的哈希表结构（hash table），用于存储键值对。哈希表通过哈希函数将键映射到一个数组索引，以快速查找、插入或删除键值对。

具体来说，代码的作用如下：

1. **`ht_item`结构体：**
    这个结构体用来表示哈希表中的每一个元素，包含两个成员：

   - `char* key`：表示哈希表中元素的键，通常是字符串类型。
   - `char* value`：表示哈希表中元素的值，同样是字符串类型。

   通过这个结构体，我们可以在哈希表中存储一对键值对。

2. **`ht_hash_table`结构体：**
    这个结构体用来表示哈希表本身，包含以下成员：

   - `int size`：哈希表的容量，即存储桶（bucket）的数量，也就是哈希表数组的大小。
   - `int count`：当前哈希表中存储的键值对的数量。
   - `ht_item** items`：这是一个指向`ht_item`结构体指针的指针。它是一个数组，每个元素都是一个指向`ht_item`的指针。使用双指针（`ht_item**`）是为了能够实现动态的内存分配和修改，因为哈希表的大小在运行时可能会发生变化。双指针使得我们能够操作指向`ht_item`结构体的指针数组，并能够插入、删除或修改哈希表中的每一个键值对。

------

### `typedef`的作用

`typedef`是C语言中的关键字，用来为已有的类型定义一个新的名称。它的作用是简化代码，使得类型名称更具可读性或更容易理解。例如：

- `typedef struct { ... } ht_item;`：定义了一个新的类型名称`ht_item`，实际上它是一个结构体类型，可以通过`ht_item`来声明结构体变量，而不需要每次都写出完整的结构体定义。
- `typedef struct { ... } ht_hash_table;`：定义了一个新的类型名称`ht_hash_table`，可以用来声明哈希表类型的变量。

这样做的好处是使得代码更简洁，易于理解。

------

### 为什么`ht_hash_table`中的`ht_item** items`是双指针

`ht_item** items`是一个指向指针的指针，即**双指针**。这样设计有以下几个原因：

1. **动态数组**：哈希表的大小是动态的，因此需要使用指针来存储数组的地址。`ht_item**`指向的是一组`ht_item*`指针，`ht_item*`指向实际的`ht_item`结构体。使用双指针的目的是为了能够在哈希表需要扩容时，动态地分配新的内存空间，并能改变指向的数组位置。
2. **哈希冲突处理**：当多个键值对经过哈希函数映射到同一个索引时，会发生哈希冲突。在这种情况下，通常会使用链式法（通过链表）来解决冲突。双指针的设计使得每个数组索引位置可以指向一个链表的头节点，从而处理哈希冲突。
3. **插入和删除操作**：使用双指针可以方便地进行插入和删除操作。通过修改`ht_item*`指针的指向，可以灵活地管理内存，避免内存泄漏或无效数据。

总之，`ht_item** items`中的双指针设计可以提供更高的灵活性，以应对动态变化的哈希表大小以及处理哈希冲突等问题。



在哈希表（Hash Table）中，**桶（bucket）**是用来存储哈希表中键值对的容器。具体来说，哈希表通过一个哈希函数将键（key）映射到一个数组索引位置，而这个位置就对应着一个桶。桶的作用是存储那些被哈希函数映射到相同索引位置的元素。

### 桶的概念

1. **桶的作用**
    每个桶在哈希表中都是一个存储位置，它可以容纳一个或多个键值对。因为哈希函数可能会将不同的键映射到相同的桶，这种情况叫做**哈希冲突（collision）**。为了解决这个问题，常见的做法是使用链表或者其他数据结构在桶内存储多个元素。

2. **哈希冲突与解决方法**

   - **链式法（Chaining）**：当多个键被哈希函数映射到同一个桶时，哈希表会在该桶内使用链表来存储所有的键值对。每个桶可能会是一个链表的头节点，链表中的每个节点存储一个`ht_item`结构体。
   - **开放寻址法（Open Addressing）**：另一种常见的解决哈希冲突的方法是通过开放寻址技术。当发生冲突时，哈希表会在数组的其他位置寻找空的存储桶来存储冲突的元素，常见的开放寻址方法有线性探测、二次探测等。

3. **桶与哈希函数**
    哈希表的大小（`size`）决定了哈希表中有多少个桶。每个桶的索引是通过哈希函数计算得出的，通常使用如下公式：

   $$\text{index} = \text{hash}(key) \mod \text{size}$$

   其中，`hash(key)`是对键执行哈希函数计算得到的哈希值，而`mod size`操作则确保哈希值映射到哈希表中的合法桶索引。

4. **扩容和调整**
    当哈希表中的元素过多时，可能会导致哈希冲突频发，性能下降。此时，哈希表可以进行**扩容**，即增加桶的数量。扩容时，哈希表会重新计算每个键的位置，并将它们重新分布到新的桶中。

### 示例

假设我们有一个哈希表，`size`为5（即有5个桶）。如果哈希表中插入了3个键值对，其中两个键通过哈希函数计算后映射到同一个桶（发生哈希冲突），则这个桶可能就变成一个链表来存储这两个键值对。

- 键`"apple"`映射到桶0，存储在该桶。
- 键`"banana"`映射到桶1，存储在该桶。
- 键`"orange"`映射到桶0，与`"apple"`发生冲突，因此被存储在桶0的链表中。

最终，哈希表的存储结构可能是这样的：

```c
桶 0: [ ("apple", "fruit"), ("orange", "fruit") ]
桶 1: [ ("banana", "fruit") ]
桶 2: []
桶 3: []
桶 4: []
```

通过桶的设计，哈希表可以高效地进行插入、查找和删除操作，而哈希函数和冲突解决机制则确保了哈希表在面对冲突时依然能够保持良好的性能。



## Initialising and deleting

We need to define initialisation functions for `ht_item`s. This function allocates a chunk of memory the size of an `ht_item`, and saves a copy of the strings `k` and `v` in the new chunk of memory. The function is marked as `static` because it will only ever be called by code internal to the hash table.



```c
// hash_table.c
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

static ht_item* ht_new_item(const char* k, const char* v) {
    ht_item* i = malloc(sizeof(ht_item));
    i->key = strdup(k);
    i->value = strdup(v);
    return i;
}
```

这个函数 `ht_new_item` 用于创建一个新的哈希表项 (`ht_item`)，并将指定的键值对（`k` 和 `v`）复制到该项中。

### 代码解析

1. **函数声明**

   ```c
   static ht_item* ht_new_item(const char* k, const char* v);
   ```

   这段代码定义了一个名为 `ht_new_item` 的静态函数，它返回一个 `ht_item*` 类型的指针，表示一个哈希表项。函数接受两个参数：

   - `k`：键（`key`），类型为 `const char*`，即指向字符常量的指针。
   - `v`：值（`value`），类型同样为 `const char*`。

   **`static`** 关键字意味着该函数仅在 `hash_table.c` 文件内有效，不能被其他文件引用。因为这个函数是哈希表内部使用的辅助函数，其他代码不需要访问它。

2. **内存分配**

   ```c
   ht_item* i = malloc(sizeof(ht_item));
   ```

   这里，使用 `malloc` 函数动态分配了一块内存，大小为 `ht_item` 结构体的大小（`sizeof(ht_item)`）。这将为新的哈希表项分配内存空间，并返回一个指向该内存的指针 `i`。指针 `i` 现在指向一个空的 `ht_item` 结构体。

3. **字符串复制**

   ```c
   i->key = strdup(k);
   i->value = strdup(v);
   ```

   `strdup` 函数用于复制字符串。在这里：

   - `i->key = strdup(k);` 这行代码将输入的 `k` 字符串复制到堆内存中，并将该内存地址赋给 `i->key`。
   - `i->value = strdup(v);` 这行代码将输入的 `v` 字符串复制到堆内存中，并将该内存地址赋给 `i->value`。

   **`strdup` 的作用**： `strdup` 是一个标准库函数，作用是：

   - 分配一块内存来存储字符串的副本。
   - 将输入的字符串复制到分配的内存中。
   - 返回指向新字符串的指针。

   使用 `strdup` 可以确保哈希表项中的键值对独立于原始字符串，避免由于原始字符串被修改或销毁而影响哈希表的数据。

4. **返回新项指针**

   ```c
   return i;
   ```

   最后，函数返回指向新创建的哈希表项（`ht_item`）的指针 `i`，此时该项已经包含了复制的键值对。

### 总结

`ht_new_item` 函数的作用是：

- 为一个新的哈希表项分配内存。
- 将传入的键值对（`k` 和 `v`）分别复制到新分配的内存中。
- 返回指向这个新创建的哈希表项的指针。

通过这个函数，哈希表可以为每个新的键值对创建独立的内存副本，保证数据的完整性。



`ht_new` initialises a new hash table. `size` defines how many items we can store. This is fixed at 53 for now. We'll expand this in the section on [resizing](https://github.com/jamesroutley/write-a-hash-table/blob/master/06-resizing). We initialise the array of items with `calloc`, which fills the allocated memory with `NULL` bytes. A `NULL` entry in the array indicates that the bucket is empty.

```c
// hash_table.c
ht_hash_table* ht_new() {
    ht_hash_table* ht = malloc(sizeof(ht_hash_table));

    ht->size = 53;
    ht->count = 0;
    ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
    return ht;
}
```

这个函数 `ht_new` 用于创建一个新的哈希表（`ht_hash_table`）并初始化其各个成员。下面我们将逐步解析这个函数的每一行代码，并特别关注 `calloc` 的作用。

### 代码解析

```c
ht_hash_table* ht_new() {
    ht_hash_table* ht = malloc(sizeof(ht_hash_table));
```

1. **分配内存**
    这行代码通过 `malloc` 为一个新的 `ht_hash_table` 结构体分配内存，`sizeof(ht_hash_table)` 返回的是 `ht_hash_table` 类型的大小（即哈希表结构的内存大小）。`malloc` 函数返回一个指向这块内存的指针，这个指针被赋值给变量 `ht`。此时，`ht` 是指向一个尚未初始化的 `ht_hash_table` 结构体的指针。

```c
    ht->size = 53;
    ht->count = 0;
```

1. 初始化大小和元素计数
   - `ht->size = 53;`：设置哈希表的初始容量为 53。哈希表的容量（`size`）是存储桶（bucket）的数量，通常选择质数作为初始容量，以减少哈希冲突的可能性。
   - `ht->count = 0;`：初始化哈希表中的元素个数（`count`）为 0，表示当前哈希表没有存储任何键值对。

```c
    ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
```

1. **分配并初始化存储桶（桶数组）**
    这行代码非常重要，使用了 `calloc` 函数来分配和初始化哈希表的存储桶数组。

   - `calloc` 函数的作用是**分配内存并将其初始化为零**。它的原型是：

     ```c
     void* calloc(size_t num, size_t size);
     ```

     其中 `num` 表示需要分配的元素数量，`size` 是每个元素的大小。

   - 在这行代码中，`calloc((size_t)ht->size, sizeof(ht_item*))` 表示为哈希表中的存储桶数组分配内存。具体来说：

     - `ht->size` 是桶的数量，这里为 53。
     - `sizeof(ht_item*)` 是每个桶的大小，`ht_item*` 是指向 `ht_item` 的指针类型，因此每个桶的大小是指针的大小（通常是 4 字节或 8 字节，取决于系统架构）。

   `calloc` 会分配 53 个指针大小的内存块，并将它们的值初始化为 `NULL`（零值）。这意味着哈希表的每个桶在初始化时都为空（没有存储任何键值对）。

```c
    return ht;
}
```

1. **返回哈希表指针**
    最后，函数返回指向新创建并初始化的哈希表的指针 `ht`。此时，哈希表的容量为 53，当前没有存储任何元素，所有的桶都为空。

### `calloc` 详解

`calloc` 函数与 `malloc` 函数的区别在于：

- `malloc` 仅仅分配内存空间，但不对其内容进行初始化，因此内存中的内容是未定义的。
- `calloc` 除了分配内存外，还会将分配的内存区域的所有字节初始化为零。

在本例中：

- `calloc((size_t)ht->size, sizeof(ht_item*))` 为哈希表的桶数组分配了 53 个元素，每个元素大小为 `sizeof(ht_item*)`，并且会将所有这些内存位置初始化为 `NULL`。这样，哈希表初始化时每个桶都为空，避免了程序在未初始化桶指针时可能发生的错误。

### 总结

`ht_new` 函数用于创建和初始化一个新的哈希表。主要步骤包括：

1. 为哈希表结构分配内存并初始化。
2. 设置哈希表的初始大小和元素计数。
3. 使用 `calloc` 为桶数组分配内存，并将所有桶的指针初始化为 `NULL`。
4. 返回指向新哈希表的指针。

`calloc` 在这里的作用是确保每个桶的初始值为 `NULL`，从而避免因未初始化的指针导致的错误。



We also need functions for deleting `ht_item`s and `ht_hash_tables`, which `free` the memory we've allocated, so we don't cause [memory leaks](https://en.wikipedia.org/wiki/Memory_leak).

```c
// hash_table.c
static void ht_del_item(ht_item* i) {
    free(i->key);
    free(i->value);
    free(i);
}


void ht_del_hash_table(ht_hash_table* ht) {
    for (int i = 0; i < ht->size; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL) {
            ht_del_item(item);
        }
    }
    free(ht->items);
    free(ht);
}
```

这段代码定义了两个函数：`ht_del_item` 和 `ht_del_hash_table`，它们用于销毁哈希表中的元素以及哈希表本身，释放相关的内存。

### 代码解析

#### `ht_del_item` 函数

```c
static void ht_del_item(ht_item* i) {
    free(i->key);
    free(i->value);
    free(i);
}
```

这个函数用于销毁一个哈希表项（`ht_item`）并释放该项所占用的内存。

1. **`free(i->key);`**
    释放 `i`（哈希表项）的 `key` 字符串所占用的内存。`key` 是通过 `strdup` 函数复制出来的字符串，`free` 用于释放其内存。
2. **`free(i->value);`**
    类似于 `key`，释放 `i`（哈希表项）的 `value` 字符串所占用的内存。
3. **`free(i);`**
    最后，释放 `i` 本身所占用的内存，即释放 `ht_item` 结构体的内存。

#### `ht_del_hash_table` 函数

```c
void ht_del_hash_table(ht_hash_table* ht) {
    for (int i = 0; i < ht->size; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL) {
            ht_del_item(item);
        }
    }
    free(ht->items);
    free(ht);
}
```

这个函数用于销毁整个哈希表，并释放哈希表及其元素的内存。

1. **遍历哈希表中的所有桶**
    `for (int i = 0; i < ht->size; i++)` 循环遍历哈希表的每个桶。`ht->size` 是哈希表的容量（即桶的数量），假设 `size = 5`，那么 `i` 从 0 到 4 遍历哈希表的 5 个桶。
2. **检查桶是否为空**
    `ht_item* item = ht->items[i];` 获取第 `i` 个桶中的元素（即 `ht_item*`）。如果该桶中存储了某个项（即 `item != NULL`），就调用 `ht_del_item(item)` 销毁该项并释放内存。
3. **释放桶数组的内存**
    `free(ht->items);` 释放存储桶指针数组的内存，`ht->items` 是指向存储桶的指针数组。
4. **释放哈希表结构本身的内存**
    `free(ht);` 最后，释放哈希表结构本身所占用的内存。

### 数值模拟

假设我们有一个哈希表 `ht`，其中包含 5 个桶（`size = 5`），并且哈希表中的某些桶已经存储了哈希表项。

#### 初始化

1. 假设哈希表大小为 5，`ht->size = 5`，`ht->items` 是一个包含 5 个元素的数组，每个元素都是一个指向 `ht_item` 的指针。初始化时，`ht->items` 的每个位置可能是 `NULL`，也可能指向一个 `ht_item` 对象。

   假设哈希表包含如下数据：

   ```
   ht->items = [
       {"apple", "fruit"},  // 存储在桶 0
       NULL,                // 桶 1 为空
       {"banana", "fruit"}, // 存储在桶 2
       {"carrot", "vegetable"}, // 存储在桶 3
       NULL                 // 桶 4 为空
   ]
   ```

#### 调用 `ht_del_hash_table(ht)`

执行 `ht_del_hash_table(ht)` 后，内存释放过程如下：

1. **桶 0**：`ht->items[0]` 指向一个 `ht_item`，它的 `key` 是 `"apple"`，`value` 是 `"fruit"`。
   - 调用 `ht_del_item` 函数，依次释放 `i->key`（即 `"apple"` 字符串）、`i->value`（即 `"fruit"` 字符串）和 `i` 本身（即 `ht_item` 结构体）。
2. **桶 1**：`ht->items[1]` 为 `NULL`，因此跳过。
3. **桶 2**：`ht->items[2]` 指向一个 `ht_item`，它的 `key` 是 `"banana"`，`value` 是 `"fruit"`。
   - 调用 `ht_del_item` 函数，依次释放 `i->key`（即 `"banana"` 字符串）、`i->value`（即 `"fruit"` 字符串）和 `i` 本身（即 `ht_item` 结构体）。
4. **桶 3**：`ht->items[3]` 指向一个 `ht_item`，它的 `key` 是 `"carrot"`，`value` 是 `"vegetable"`。
   - 调用 `ht_del_item` 函数，依次释放 `i->key`（即 `"carrot"` 字符串）、`i->value`（即 `"vegetable"` 字符串）和 `i` 本身（即 `ht_item` 结构体）。
5. **桶 4**：`ht->items[4]` 为 `NULL`，因此跳过。

#### 最终清理

- `free(ht->items)`：释放存储桶数组 `ht->items` 的内存。由于桶数组是通过 `calloc` 分配的，已被初始化为 `NULL` 或指向某些项。
- `free(ht)`：释放 `ht` 哈希表结构本身的内存。

### 总结

- `ht_del_item` 用于释放单个哈希表项的内存，包括其 `key`、`value` 和 `ht_item` 结构体本身。
- `ht_del_hash_table` 用于遍历哈希表中的所有桶，并销毁所有非空的哈希表项，最后释放存储桶数组和哈希表结构本身的内存。

We have written code which defines a hash table, and lets us create and destroy one. Although it doesn't do much at this point, we can still try it out.

```c
// main.c
#include "hash_table.h"

int main() {
    ht_hash_table* ht = ht_new();
    ht_del_hash_table(ht);
}
```

