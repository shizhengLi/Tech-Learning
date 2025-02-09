# Resizing

Link: [https://github.com/jamesroutley/write-a-hash-table/tree/master/06-resizing](https://github.com/jamesroutley/write-a-hash-table/tree/master/06-resizing) 对原作者表示感谢

Currently, our hash table has a fixed number of buckets. As more items are inserted, the table starts to fill up. This is problematic for two reasons:

1. The hash table's performance diminishes with high rates of collisions
2. Our hash table can only store a fixed number of items. If we try to store more than that, the insert function will fail.

To mitigate this, we can increase the size of the item array when it gets too full. We store the number of items stored in the hash table in the table's `count` attribute. On each insert and delete, we calculate the table's 'load', or ratio of filled buckets to total buckets. If it gets higher or lower than certain values, we resize the bucket up or down.

We will resize:

- up, if load > 0.7
- down, if load < 0.1

To resize, we create a new hash table roughly half or twice as big as the current, and insert all non-deleted items into it.

Our new array size should be a prime number roughly double or half the current size. Finding the new array size isn't trivial. To do so, we store a base size, which we want the array to be, and then define the actual size as the first prime larger than the base size. To resize up, we double the base size, and find the first larger prime, and to resize down, we halve the size and find the next larger prime.

Our base sizes start at 50. Instead of storing

We use a brute-force method to find the next prime, by checking if each successive number is prime. While brute-forcing anything sounds alarming, the number of values we actually have to check is low, and the time it takes is outweighed by the time spent re-hashing every item in the table.

## Next prime



First, let's define a function for finding the next prime. We'll do this in two new files, `prime.h` and `prime.c`.

```c
// prime.h
int is_prime(const int x);
int next_prime(int x);
```

代码实现

```c
// prime.c

#include <math.h>

#include "prime.h"


/*
 * Return whether x is prime or not
 *
 * Returns:
 *   1  - prime
 *   0  - not prime
 *   -1 - undefined (i.e. x < 2)
 */
int is_prime(const int x) {
    if (x < 2) { return -1; }
    if (x < 4) { return 1; }
    if ((x % 2) == 0) { return 0; }
    for (int i = 3; i <= floor(sqrt((double) x)); i += 2) {
        if ((x % i) == 0) {
            return 0;
        }
    }
    return 1;
}


/*
 * Return the next prime after x, or x if x is prime
 */
int next_prime(int x) {
    while (is_prime(x) != 1) {
        x++;
    }
    return x;
}
```

目前，计算质数的方式使用了**暴力法**，即逐个检查数字是否为质数，这在处理较大的数字时效率较低。下面是当前实现的简要分析：

### 当前实现分析：

1. **`is_prime`** 函数通过检查数字是否为2的倍数，接着使用奇数检查，从3开始逐一检查到数字的平方根。这种方法的时间复杂度是 $O(\sqrt{n})$，对于每个数字，时间复杂度是比较高的，尤其是当数字较大时。
2. **`next_prime`** 函数通过循环调用 `is_prime` 来找到下一个质数，直到找到一个质数。这导致了重复的检查，可能会比较低效。

### 更优的方案：

可以使用一些更高效的方法来优化质数的查找，例如**埃拉托斯特尼筛法**（Sieve of Eratosthenes），它可以预先计算出范围内所有的质数，并且其时间复杂度为$O(n \log \log n)$，对于大范围内的质数计算要比暴力方法高效很多。

不过，考虑到我们只需要找到单个下一个质数，可以使用一些小技巧来优化原有方法。

### 优化方法：

1. **避免重复检查**：我们可以将已知的质数存储在一个数组中，然后在查找下一个质数时，仅检查没有出现在质数数组中的数。
2. **跳过偶数**：除了2，所有质数都是奇数，因此在检查下一个数时，跳过偶数。

### 优化后的C语言实现：

```c
#include <math.h>
#include <stdbool.h>

// 判断一个数字是否是质数
bool is_prime(const int x) {
    if (x < 2) return false;
    if (x == 2) return true;  // 2是质数
    if (x % 2 == 0) return false;  // 排除偶数

    for (int i = 3; i <= sqrt(x); i += 2) {
        if (x % i == 0) return false;
    }

    return true;
}

// 查找下一个质数
int next_prime(int x) {
    if (x <= 2) return 2;  // 如果x小于等于2，则下一个质数是2

    x = (x % 2 == 0) ? x + 1 : x + 2;  // 如果x是偶数，先跳过，直接开始从下一个奇数检查

    while (!is_prime(x)) {
        x += 2;  // 每次跳过偶数，检查下一个奇数
    }

    return x;
}
```

### 说明：

1. **`is_prime` 优化**：
   - 直接排除小于2的数字。
   - 对于偶数，直接返回 `false`，避免了不必要的检查。
   - 检查从3开始，且每次步长为2（即检查奇数），大大减少了需要检查的数字数量。
2. **`next_prime` 优化**：
   - 如果给定的 `x` 是偶数，直接跳到下一个奇数。
   - 通过每次跳过偶数，避免了大量的无效检查，提升了效率。

### 改进后算法的优点：

- **性能提升**：对于大数，优化后的方法避免了检查所有的数字，而只检查奇数，提高了效率。
- **简洁性**：通过跳过偶数，减少了多余的计算，使得代码更加简洁。

这种方法相比原始的暴力法，更高效，也能有效减少不必要的计算。如果需要进一步提升性能，可以使用埃拉托斯特尼筛法，特别是当处理大范围质数时。

## ht_new_sized



Next, we need to update our `ht_new` function to support creating hash tables of a certain size. To do this, we'll create a new function, `ht_new_sized`. We change `ht_new` to call `ht_new_sized` with the default starting size.



```c
// hash_table.c
static ht_hash_table* ht_new_sized(const int base_size) {
    ht_hash_table* ht = xmalloc(sizeof(ht_hash_table));
    ht->base_size = base_size;

    ht->size = next_prime(ht->base_size);

    ht->count = 0;
    ht->items = xcalloc((size_t)ht->size, sizeof(ht_item*));
    return ht;
}


ht_hash_table* ht_new() {
    return ht_new_sized(HT_INITIAL_BASE_SIZE);
}
```



### 代码解析：

这段代码是一个用于初始化哈希表的函数，主要通过 `ht_new_sized` 来创建一个指定初始大小的哈希表，或者通过 `ht_new` 创建一个使用默认初始大小的哈希表。

1. **`ht_new_sized` 函数**：

   ```c
   static ht_hash_table* ht_new_sized(const int base_size) {
       ht_hash_table* ht = xmalloc(sizeof(ht_hash_table));  // 1
       ht->base_size = base_size;
   
       ht->size = next_prime(ht->base_size);  // 2
   
       ht->count = 0;
       ht->items = xcalloc((size_t)ht->size, sizeof(ht_item*));  // 3
       return ht;
   }
   ```

   - **第1行：** `ht_hash_table* ht = xmalloc(sizeof(ht_hash_table));`
     - 这行代码使用 `xmalloc` 为哈希表结构体 `ht_hash_table` 分配内存。`xmalloc` 是一个自定义的内存分配函数，通常在很多程序中，`xmalloc` 被设计为在分配内存失败时抛出错误（或者终止程序），而不是像标准的 `malloc` 那样返回 `NULL`。这有助于简化错误处理，因为程序会在内存分配失败时立刻停止。
   - **第2行：** `ht->size = next_prime(ht->base_size);`
     - 这里调用了 `next_prime` 函数，获取一个大于或等于给定 `base_size` 的质数，并将其赋值给哈希表的大小（`size`）。之所以使用质数，是因为质数可以帮助减少哈希冲突的发生，提升哈希表的性能。
   - **第3行：** `ht->items = xcalloc((size_t)ht->size, sizeof(ht_item*));`
     - 这里使用 `xcalloc` 为哈希表的桶（`items`）数组分配内存。`xcalloc` 不仅分配内存，还会将其初始化为零。我们为 `ht->size` 个桶分配了内存，每个桶的大小是 `sizeof(ht_item*)`（即每个桶是一个指向 `ht_item` 结构体的指针）。

2. **`ht_new` 函数**：

   ```c
   ht_hash_table* ht_new() {
       return ht_new_sized(HT_INITIAL_BASE_SIZE);
   }
   ```

   - `ht_new` 调用 `ht_new_sized`，并传入一个初始的基准大小 `HT_INITIAL_BASE_SIZE`，这是一个预定义的常量（可能是50或其他合理的初始值）。它返回一个新创建的哈希表。

------

### `xcalloc` 解析：

`xcalloc` 是一个自定义的内存分配函数，它通常有以下特性：

1. **分配内存**： `xcalloc` 与标准库函数 `calloc` 相似，作用是为指定大小的内存块分配内存。不同的是，`xcalloc` 可能会对内存分配失败的情况进行特殊处理，比如抛出异常或直接终止程序。
2. **初始化内存**： `calloc` 函数会将分配的内存初始化为0。这意味着 `ht->items` 数组的每个元素都会被初始化为 `NULL`，避免出现未初始化的指针。使用 `xcalloc` 可以确保所有桶的初始状态都是“空”的。

### 为什么使用 `xcalloc`：

1. **确保内存初始化为零**：
   - 使用 `xcalloc` 可以确保 `ht->items` 数组的每个元素都初始化为 `NULL`，这对于哈希表的正确操作至关重要，因为每个桶的初始状态应该是空的。如果使用 `malloc` 而不初始化，可能会导致指针随机值，导致程序出现不可预料的错误。
2. **简化错误处理**：
   - 与标准的 `malloc` 不同，`xcalloc` 可能会在内存分配失败时停止程序执行。这样避免了在内存分配失败的情况下继续执行代码，从而减少了潜在的内存错误。`xmalloc` 和 `xcalloc` 使得内存管理更加健壮，程序的错误处理也变得更加简单。

### 小结：

- **`ht_new_sized`**：该函数创建一个新哈希表，初始化其大小、容量，并将桶数组内存初始化为0。
- **`xcalloc`**：与 `calloc` 类似，`xcalloc` 不仅分配内存，还将其初始化为零，并且可能提供额外的错误处理机制。



## resize



Now we have all the parts we need to write our resize function.

In our resize function, we check to make sure we're not attempting to reduce the size of the hash table below its minimum. We then initialise a new hash table with the desired size. All non `NULL` or deleted items are inserted into the new hash table. We then swap the attributes of the new and old hash tables before deleting the old.



```c
// hash_table.c
static void ht_resize(ht_hash_table* ht, const int base_size) {
    if (base_size < HT_INITIAL_BASE_SIZE) {
        return;
    }
    ht_hash_table* new_ht = ht_new_sized(base_size);
    for (int i = 0; i < ht->size; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) {
            ht_insert(new_ht, item->key, item->value);
        }
    }

    ht->base_size = new_ht->base_size;
    ht->count = new_ht->count;

    // To delete new_ht, we give it ht's size and items 
    const int tmp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmp_size;

    ht_item** tmp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = tmp_items;

    ht_del_hash_table(new_ht);
}
```

### 函数的目的：

`ht_resize` 函数的目的是对哈希表进行“重新调整大小”操作。当哈希表的负载因子超过某个阈值时，需要增加哈希表的容量（扩大桶的数量）。相反，如果负载因子过低（比如空间浪费严重），则可以减小哈希表的容量。



To simplify resizing, we define two small functions for resizing up and down.

```c
// hash_table.c
static void ht_resize_up(ht_hash_table* ht) {
    const int new_size = ht->base_size * 2;
    ht_resize(ht, new_size);
}


static void ht_resize_down(ht_hash_table* ht) {
    const int new_size = ht->base_size / 2;
    ht_resize(ht, new_size);
}
```



To perform the resize, we check the load on the hash table on insert and deletes. If it is above or below predefined limits of 0.7 and 0.1, we resize up or down respectively.

To avoid doing floating point maths, we multiply the count by 100, and check if it is above or below 70 or 10.

```c
// hash_table.c
void ht_insert(ht_hash_table* ht, const char* key, const char* value) {
    const int load = ht->count * 100 / ht->size;
    if (load > 70) {
        ht_resize_up(ht);
    }
    // ...
}


void ht_delete(ht_hash_table* ht, const char* key) {
    const int load = ht->count * 100 / ht->size;
    if (load < 10) {
        ht_resize_down(ht);
    }
    // ...
}
```



Next section: [Appendix](https://github.com/jamesroutley/write-a-hash-table/blob/master/07-appendix) [Table of contents](https://github.com/jamesroutley/write-a-hash-table#contents)





# Appendix: alternative collision handling

There are two common methods for handling collisions in hash tables:

- Separate chaining
- Open addressing

## Separate chaining

Under separate chaining, each bucket contains a linked list. When items collide, they are added to the list. Methods:

- Insert: hash the key to get the bucket index. If there is nothing in that bucket, store the item there. If there is already an item there, append the item to the linked list.
- Search: hash the key to get the bucket index. Traverse the linked list, comparing each item's key to the search key. If the key is found, return the value, else return `NULL`.
- Delete: hash the key to get the bucket index. Traverse the linked list, comparing each item's key to the delete key. If the key is found, remove the item from the linked list. If there is only one item in the linked list, place the `NULL` pointer in the bucket, to indicate that it is empty.

This has the advantage of being simple to implement, but is space inefficient. Each item has to also store a pointer to the next item in the linked list, or the `NULL` pointer if no items come after it. This is space wasted on bookkeeping, which could be spent on storing more items.

### Separate Chaining (分离链式法)

**Separate chaining** 是哈希表解决冲突的一种常见方法，它通过在每个桶（bucket）中使用链表（linked list）来存储碰撞的项。在发生哈希碰撞时，新项会被添加到链表中。

### 关键概念：

1. **桶（bucket）**：哈希表的每个索引位置称为桶。当多个项发生碰撞时，它们会被存储在同一个桶中，而该桶会变成一个链表。
2. **链表（linked list）**：当一个桶中存放多个项时，所有项会通过链表链接在一起。每个链表节点包含一个项（键值对），以及指向下一个节点的指针，直到链表结束。

### 操作方法：

1. **插入（Insert）**：
   - 计算哈希值，找到对应的桶。
   - 如果桶为空（没有冲突），直接将该项放入桶中。
   - 如果桶已经有项（发生了碰撞），则将新项添加到链表的末尾。
2. **查找（Search）**：
   - 计算哈希值，找到对应的桶。
   - 遍历桶中的链表，逐个比较链表中的键（key）与待查找的键是否匹配。
   - 如果找到匹配项，返回对应的值；如果遍历完链表还没有找到，则返回 `NULL`。
3. **删除（Delete）**：
   - 计算哈希值，找到对应的桶。
   - 遍历链表，查找待删除的项。
   - 如果找到匹配的项，将其从链表中删除。
   - 如果链表中只有一个项，删除后将桶中的指针设置为 `NULL`，表示该桶为空。

### 优点：

- **简单实现**：分离链式法实现起来相对简单，只需在每个桶中维护一个链表。
- **性能稳定**：在哈希表的负载因子较低时，查找、插入、删除的操作平均时间复杂度为 O(1)；即使发生碰撞，链表的插入、删除操作也不会影响整体效率。

### 缺点：

- **空间浪费**：每个项不仅要存储其本身的键和值，还需要存储一个指向链表下一个节点的指针。这增加了额外的存储开销。
- **较差的缓存局部性**：由于链表中的元素可能分散存储在内存中，可能会导致访问时缓存不命中，影响性能。
- **性能退化**：如果哈希函数不好，或者哈希表的负载因子过高，链表可能变得非常长，从而导致性能退化到 O(n)，即线性时间复杂度。

### 举例说明：

假设我们有一个哈希表，其大小为 4，使用链表来处理碰撞，哈希函数是 `hash(key) % 4`，并且我们插入以下键值对：

- `("apple", 1)`
- `("banana", 2)`
- `("cherry", 3)`
- `("date", 4)`
- `("fig", 5)`

1. 插入操作：
   - `"apple"` 的哈希值为 `hash("apple") % 4 = 1`，放入索引 1。
   - `"banana"` 的哈希值为 `hash("banana") % 4 = 2`，放入索引 2。
   - `"cherry"` 的哈希值为 `hash("cherry") % 4 = 3`，放入索引 3。
   - `"date"` 的哈希值为 `hash("date") % 4 = 0`，放入索引 0。
   - `"fig"` 的哈希值为 `hash("fig") % 4 = 1`，发生碰撞，放入索引 1 对应链表的末尾。

此时哈希表如下：

| 索引 | 链表                      |
| ---- | ------------------------- |
| 0    | ("date", 4)               |
| 1    | ("apple", 1) → ("fig", 5) |
| 2    | ("banana", 2)             |
| 3    | ("cherry", 3)             |

2. **查找操作**：

- 查找 `"fig"`：计算哈希值 `hash("fig") % 4 = 1`，然后遍历链表，找到 `"fig"`，返回 5。

3. **删除操作**：

- 删除 `"apple"`：计算哈希值 `hash("apple") % 4 = 1`，找到 `"apple"`，并将其从链表中删除。此时链表变为：`("fig", 5)`。

### 结论：

- **Separate chaining** 方法非常适合应对哈希表中的冲突问题，尤其在负载因子较低、碰撞相对较少的情况下，能够提供高效的性能。
- 但是，由于空间开销和缓存问题，链表法的效率受限，尤其在哈希表负载较高时，可能导致性能严重下降。



## Open addressing



Open addressing aims to solve the space inefficiency of separate chaining. When collisions happen, the collided item is placed in some other bucket in the table. The bucket that the item is placed into is chosen according some predetermined rule, which can be repeated when searching for the item. There are three common methods for choosing the bucket to insert a collided item into.

#### Linear probing.

When a collision occurs, the index is incremented and the item is put in the next available bucket in the array. Methods:

- Insert: hash the key to find the bucket index. If the bucket is empty, insert the item there. If it is not empty, repeatedly increment the index until an empty bucket is found, and insert it there.
- Search: hash the key to find the bucket index. Repeatedly increment the index, comparing each item's key to the search key, until an empty bucket is found. If an item with a matching key is found, return the value, else return `NULL`.
- Delete: hash the key to find the bucket index. Repeatedly increment the index, comparing each item's key to the delete key, until an empty bucket is found. If an item with a matching key is found, delete it. Deleting this item breaks the chain, so we have no choice but to reinsert all items in the chain after the deleted item.

Linear probing offers good [cache performance](https://en.wikipedia.org/wiki/Locality_of_reference), but suffers from clustering issues. Putting collided items in the next available bucket can lead to long contiguous stretches of filled buckets, which need to be iterated over when inserting, searching or deleting.

#### Quadratic probing.

Similar to linear probing, but instead of putting the collided item in the next available bucket, we try to put it in the buckets whose indexes follow the sequence: `i, i + 1, i + 4, i + 9, i + 16, ...`, where `i` is the original hash of the key. Methods:

- Insert: hash the key to find the bucket index. Follow the probing sequence until an empty or deleted bucket is found, and insert the item there.
- Search: hash the key to find the bucket index. Follow the probing sequence, comparing each item's key to the search key until an empty bucket is found. If a matching key is found, return the value, else return `NULL`.
- Delete: we can't tell if the item we're deleting is part of a collision chain, so we can't delete the item outright. Instead, we just mark it as deleted.

Quadratic probing reduces, but does not remove, clustering, and still offers decent cache performance.

#### Double hashing.

Double hashing aims to solve the clustering problem. To do so, we use a second hash function to choose a new index for the item. Using a hash function gives us a new bucket, the index of which should be evenly distributed across all buckets. This removes clustering, but also removes any boosted cache performance from locality of reference. Double hashing is a common method of collision management in production hash tables, and is the method we implement in this tutorial.



## 致谢

https://github.com/jamesroutley/write-a-hash-table/blob/master/LICENSE.md

>MIT License
>
>Copyright (c) 2017 James Routley
>
>Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
>
>The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
>
>THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.



## 后记

这个项目完结了，正如原作者所说，花费一两个小时就搞定了。知道如何实现还是很重要的，尤其是和只看书本不动手的人相比。

2025年1月18日21点34分于上海。在OpenAI o1辅助下完成。

