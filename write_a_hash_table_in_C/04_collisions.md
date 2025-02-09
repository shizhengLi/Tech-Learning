# Handling collisions

Link：[https://github.com/jamesroutley/write-a-hash-table/tree/master/04-collisions](https://github.com/jamesroutley/write-a-hash-table/tree/master/04-collisions)

对原作者表示感谢

Hash functions map an infinitely large number of inputs to a finite number of outputs. Different input keys will map to the same array index, causing bucket collisions. Hash tables must implement some method of dealing with collisions.

Our hash table will handle collisions using a technique called open addressing with double hashing. Double hashing makes use of two hash functions to calculate the index an item should be stored at after `i` collisions.

For an overview of other types of collision resolution, see the [appendix](https://github.com/jamesroutley/write-a-hash-table/blob/master/07-appendix).

## Double hashing

在哈希表中，**哈希冲突**（Hash Collisions）是不可避免的，因为哈希函数将无限多个可能的输入映射到有限的桶中。当不同的输入映射到相同的桶时，就发生了冲突。为了处理这种情况，哈希表需要实现冲突解决策略。

### 开放寻址法（Open Addressing）

**开放寻址法**是一种常见的哈希冲突解决方法。它的核心思想是，当发生冲突时，**不使用额外的数据结构（如链表）来存储冲突项，而是将冲突的元素寻找另一个空桶（位置）存放**。换句话说，所有的元素都存储在哈希表的数组中，并通过某种策略寻找冲突项的下一个合适位置。

在开放寻址法中，**查找冲突解决方法**依赖于以下几个关键因素：

- **探查序列**（Probe Sequence）：即冲突发生时，如何继续查找下一个空桶。
- **探查方式**：即如何计算下一个尝试的桶位置。

### Double Hashing（双重哈希）

**双重哈希**（Double Hashing）是开放寻址法中的一种特定方法，它使用**两个哈希函数**来解决冲突。具体来说，双重哈希会根据第一个哈希函数计算出一个初始索引，如果该位置已经被占用，则使用第二个哈希函数来计算一个新的桶位置，从而找到下一个空桶。这个过程会持续进行，直到找到一个空桶或者表满为止。

#### 双重哈希的具体步骤：

1. **第一个哈希函数**：给定输入的键，使用第一个哈希函数 `h1(key)` 计算出一个初始的桶索引。
2. 检查冲突：如果该索引位置已经被占用（即发生冲突），则计算下一个桶位置：
   - 使用第二个哈希函数 `h2(key)` 计算步长，并将步长加到第一个哈希值上，得到新的桶位置。
3. **重复过程**：重复上述步骤直到找到一个空桶或者遍历完整个哈希表。

公式表示如下：

- 初始位置：`index = h1(key)`

- 如果发生冲突：

  ```c
  index = (index + i * h2(key)) % table_size
  ```

  - 其中 `i` 是当前探查的次数（从 `i = 1` 开始递增），`h2(key)` 是第二个哈希函数计算得到的步长。

#### 为什么使用双重哈希？

- **减少冲突**：双重哈希通过引入第二个哈希函数，降低了哈希冲突的概率。与线性探查或二次探查不同，双重哈希的步长是动态的，能够更加均匀地分布元素，从而减少了聚集现象（即探查序列过于集中在某些位置）。
- **提高性能**：由于步长是由第二个哈希函数计算的，双重哈希通常比线性探查和二次探查更高效，能够更快地找到空桶。

### 举个例子

假设我们有一个哈希表，大小为 `10`（即 `num_buckets = 10`），并使用两个哈希函数：

- `h1(key) = key % 10`（计算初始桶位置）
- `h2(key) = 7 - (key % 7)`（计算步长）

如果我们要插入键 `key = 32`：

1. 计算第一个哈希值：
   - `h1(32) = 32 % 10 = 2`
   - 所以初始位置是桶 `2`。
2. 检查冲突：如果桶 2已经被占用，则使用第二个哈希函数计算步长：
   - `h2(32) = 7 - (32 % 7) = 7 - 4 = 3`
3. 如果发生冲突，我们就计算下一个桶位置：
   - 第一次探查：`index = (2 + 1 * 3) % 10 = 5`
   - 第二次探查：`index = (2 + 2 * 3) % 10 = 8`
   - 以此类推，直到找到一个空桶为止。

### 开放寻址法中的其他探查方式

除了双重哈希外，开放寻址法还可以使用其他的探查策略，如：

- **线性探查**（Linear Probing）：每次冲突后，尝试检查下一个桶，即 `index = (index + 1) % table_size`。
- **二次探查**（Quadratic Probing）：每次冲突后，步长按二次方增长，即 `index = (index + i^2) % table_size`，其中 `i` 是当前探查的次数。

### 总结

- **开放寻址法**通过直接在哈希表内查找空桶来解决哈希冲突。
- **双重哈希**是其中的一种策略，它使用两个哈希函数来确定哈希表中的桶位置，能够有效减少冲突并提高性能。
- 通过这种方法，哈希表能够在处理冲突时仍然保持较高的查询性能，避免了链表等结构的额外开销。

在实际应用中，双重哈希通常比线性探查和二次探查更优，特别是在元素数量较大时，能够更好地避免聚集现象和哈希冲突。



The index that should be used after `i` collisions is given by:

```c
index = [hash_a(string) + i * hash_b(string)] % num_buckets
```

We see that if no collisions have occurred, `i = 0`, so the index is just `hash_a` of the string. If a collision happens, the index is modified by the `hash_b`.

It is possible that `hash_b` will return 0, reducing the second term to 0. This will cause the hash table to try to insert the item into the same bucket over and over. We can mitigate this by adding 1 to the result of the second hash, making sure it's never 0.

```c
index = (hash_a(string) + i * (hash_b(string) + 1)) % num_buckets
```



## Implementation

```c
// hash_table.c
static int ht_get_hash(
    const char* s, const int num_buckets, const int attempt
) {
    const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
    const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);
    return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}
```



这个 `ht_get_hash` 函数是用于在哈希表中计算一个元素的哈希值，并且结合了**双重哈希**技术，用于在发生哈希冲突时找到下一个桶的位置。

### 函数参数

1. **`s`**: 输入的字符串键（key），我们将基于这个字符串计算哈希值。
2. **`num_buckets`**: 哈希表中桶的数量，即哈希表的容量。这个值用来限制哈希值的范围，确保计算的索引在合法范围内。
3. **`attempt`**: 这是当前的探查次数，表示我们要计算的是第几次冲突发生后的位置。每次冲突时，我们都会基于这个 `attempt` 来调整哈希位置。

### 函数逻辑解析

1. **计算 `hash_a`**：
    首先调用 `ht_hash(s, HT_PRIME_1, num_buckets)` 计算第一个哈希值。这个哈希值是基于字符串 `s` 和一个质数 `HT_PRIME_1` 计算的，并且将结果限制在 `num_buckets` 以内。`HT_PRIME_1` 是一个预定义的质数，它的作用是帮助生成更均匀的哈希分布，减少冲突。

   ```c
   const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
   ```

2. **计算 `hash_b`**：
    然后，使用另一个质数 `HT_PRIME_2` 计算第二个哈希值 `hash_b`，这与 `hash_a` 使用不同的质数来确保更好的分布。`hash_b` 也是基于字符串 `s` 和质数 `HT_PRIME_2` 计算的，且同样将其限制在 `num_buckets` 范围内。

   ```c
   const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);
   ```

3. **计算最终哈希值**：
    在发生冲突时，哈希表需要根据探查次数来决定下一个桶的位置。`hash_a` 给出了一个初步的位置，但如果发生冲突，我们就要用 `hash_b` 来调整步长。`attempt` 参数表示我们当前进行的是第几次探查（从 `i=0` 开始），每次冲突后，我们就通过增加 `attempt * (hash_b + 1)` 来调整位置。

   具体来说：

   - `(attempt * (hash_b + 1))`：每次探查时，步长是 `(hash_b + 1)` 的倍数，`hash_b + 1` 用来确保步长不为零，这样可以避免死循环。
   - `(hash_a + (attempt * (hash_b + 1))) % num_buckets`：最终的哈希值是在原始的 `hash_a` 基础上，通过探查次数 `attempt` 和步长来调整的，最终得到的结果会被取模 `num_buckets`，确保哈希值的范围在合法的桶范围内。

   ```c
   return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
   ```

### 双重哈希的作用

通过使用 `hash_a` 和 `hash_b` 两个不同的哈希值，我们能够在发生冲突时更均匀地分布桶的位置，避免聚集现象。具体来说：

- `hash_a` 决定了哈希表中某个元素的初始位置。
- 如果发生冲突，`hash_b` 用来计算步长，并且随着探查次数的增加，步长会逐渐增大，从而使得探查位置不会停留在某个局部区域。

### 举个例子

假设我们有如下的输入：

- `s = "key123"`
- `num_buckets = 10`（哈希表的大小）
- `attempt = 0`（第一次探查）

1. 首先计算 `hash_a`：

   ```c
   const int hash_a = ht_hash("key123", HT_PRIME_1, 10);
   ```

   假设 `hash_a = 4`。

2. 然后计算 `hash_b`：

   ```c
   const int hash_b = ht_hash("key123", HT_PRIME_2, 10);
   ```

   假设 `hash_b = 3`。

3. 最后，计算最终的位置：

   ```c
   return (4 + (0 * (3 + 1))) % 10 = (4 + 0) % 10 = 4;
   ```

如果发生冲突，我们会将 `attempt` 增加，假设进行第一次冲突的处理（`attempt = 1`）：

1. 计算新的位置：

   ```c
   return (4 + (1 * (3 + 1))) % 10 = (4 + 4) % 10 = 8;
   ```

所以，如果桶位置 `4` 被占用，第二次探查的位置将是 `8`，避免了线性探查可能引起的聚集现象。

### 总结

- `ht_get_hash` 函数结合了两个哈希函数来计算哈希值，使用双重哈希策略来处理冲突。
- 第一个哈希值 `hash_a` 给出初步的位置，如果发生冲突，第二个哈希值 `hash_b` 作为步长计算新的位置。
- 通过这种方法，哈希表能够更均匀地分配键值对，减少冲突的概率，并提高查找效率。

Next section: [Hash table methods](https://github.com/jamesroutley/write-a-hash-table/blob/master/05-methods) [Table of contents](https://github.com/jamesroutley/write-a-hash-table#contents)

