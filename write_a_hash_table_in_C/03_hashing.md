# Hash function

Link: [https://github.com/jamesroutley/write-a-hash-table/tree/master/03-hashing](https://github.com/jamesroutley/write-a-hash-table/tree/master/03-hashing)

本文是学习笔记，对原作者表示感谢。

In this section, we'll write our hash function.

The hash function we choose should:

- Take a string as its input and return a number between `0` and `m`, our desired bucket array length.
- Return an even distribution of bucket indexes for an average set of inputs. If our hash function is unevenly distributed, it will put more items in some buckets than others. This will lead to a higher rate of [collisions](https://github.com/jamesroutley/write-a-hash-table/tree/master/03-hashing#pathological-data). Collisions reduce the efficiency of our hash table.



## Algorithm

We'll make use of a generic string hashing function, expressed below in pseudocode.

```c
function hash(string, a, num_buckets):
    hash = 0
    string_len = length(string)
    for i = 0, 1, ..., string_len:
        hash += (a ** (string_len - (i+1))) * char_code(string[i])
    hash = hash % num_buckets
    return hash
```

This hash function has two steps:

1. Convert the string to a large integer
2. Reduce the size of the integer to a fixed range by taking its remainder `mod` `m`

The variable `a` should be a prime number larger than the size of the alphabet. We're hashing ASCII strings, which has an alphabet size of 128, so we should choose a prime larger than that.

`char_code` is a function which returns an integer which represents the character. We'll use ASCII character codes for this.

这段伪代码描述了一个字符串哈希函数，目的是将一个字符串映射到一个固定范围的整数值（哈希值）。这种类型的哈希函数通常用于哈希表中，用来将键映射到哈希表中的索引位置。

### 代码功能解释

#### `hash(string, a, num_buckets)` 函数

这个函数将输入的字符串 `string` 通过两个步骤转换为一个哈希值。我们来逐步分析这段代码的每个部分：

```c
function hash(string, a, num_buckets):
    hash = 0
    string_len = length(string)
```

- 初始化变量：
  - `hash`：初始化为 0，这是用来存储最终哈希值的变量。
  - `string_len`：获取字符串的长度。

```c
    for i = 0, 1, ..., string_len:
        hash += (a ** (string_len - (i+1))) * char_code(string[i])
```

- 遍历字符串并计算哈希值：
  - 这个循环遍历字符串的每个字符，从 `i = 0` 到 `i = string_len - 1`。
  - `char_code(string[i])`：获取字符串 `string` 中第 `i` 个字符的 ASCII 值。例如，对于字符 `'A'`，`char_code('A')` 返回的是 65。
  - `(a ** (string_len - (i+1)))`：将 a 的幂次与字符的 ASCII 值相乘。a是一个大于字符集大小的素数，这里 a用于在每次计算时加权字符的位置。它确保字符在哈希计算中的贡献随着字符串的位置逐渐减小。
    - 例如，如果 `a = 31`，`string_len = 4`，则字符串中的第一个字符（索引 `0`）会被加权为 `a^(4-1)`，即 `a^3`，第二个字符加权为 `a^2`，依此类推。
  - `hash += (a ** (string_len - (i+1))) * char_code(string[i])`：每个字符的加权 ASCII 值都会累加到 `hash` 中，最终得到一个大的整数。

```c
    hash = hash % num_buckets
```

- 将结果映射到固定范围：
  - `hash % num_buckets`：通过对哈希值取模操作，将哈希值限制在 `[0, num_buckets-1]` 的范围内，确保哈希值对应的哈希表的索引不超过桶的数量 `num_buckets`。这个操作将哈希值从一个大的范围缩小到我们实际需要的范围。

```c
    return hash
```

- 返回最终计算出的哈希值。

### 关键要素解释

1. **`a` 的作用**：
   - `a` 是一个素数，它用作权重因子（即字符位置的加权系数）。选择一个大于字符集大小（例如 128，对于 ASCII 字符集）的素数可以减少哈希冲突的概率。
   - 在这个函数中，`a` 的值决定了每个字符对最终哈希值的影响程度。例如，`a` 越大，字符之间的差异会变得更加显著，从而使得不同字符串的哈希值差异更加明显。
2. **`char_code(string[i])`**：
   - 该函数返回字符串中第 `i` 个字符的 ASCII 码。例如，`char_code('A')` 返回 65，`char_code('a')` 返回 97，`char_code('0')` 返回 48 等。
3. **哈希值的计算**：
   - 通过将字符串中的每个字符的 ASCII 值与其在字符串中的位置（通过幂次 `a^(string_len - (i+1))` 加权）相乘，最终得出一个很大的整数值。这种方式使得即使字符串中的字符顺序不同，哈希值也能有所不同，从而减少了哈希冲突的可能性。
4. **模运算**：
   - `hash % num_buckets` 确保哈希值被限制在 `0` 到 `num_buckets-1` 的范围内，这样就能适应哈希表的索引大小。模运算是将一个大数映射到一个较小范围的常用方法，确保哈希表的索引位置不会超出桶的数量。

### 举个例子

假设我们选择了 `a = 31`（素数）和 `num_buckets = 10`，并计算字符串 `"ABC"` 的哈希值。

1. `string_len = 3`，因此我们有 3 个字符。

2. `char_code('A') = 65, char_code('B') = 66, char_code('C') = 67`

3. 哈希值计算过程：

   - 第一个字符 A：
     - `a^(3-1) = a^2 = 31^2 = 961`
     - `hash += 961 * 65 = 62465`
   - 第二个字符 B：
     - `a^(3-2) = a^1 = 31`
     - `hash += 31 * 66 = 2046`
   - 第三个字符 C：
     - `a^(3-3) = a^0 = 1`
     - `hash += 1 * 67 = 67`
   - 计算结果：
     - `hash = 62465 + 2046 + 67 = 64578`

4. 对 

   ```c
   num_buckets = 10
   ```

    取模：

   - `hash % 10 = 64578 % 10 = 8`

最终，字符串 `"ABC"` 的哈希值是 `8`。

### 总结

这个哈希函数通过以下两个步骤实现字符串到固定范围整数的映射：

1. **将字符串转换为一个大整数**，该整数是通过加权每个字符的 ASCII 值得到的。
2. **将大整数映射到一个固定范围**，通过对哈希值取模 `num_buckets`，确保其可以作为哈希表的有效索引。

这种方法的优点在于：

- 通过加权每个字符的位置，减少了不同字符串映射到相同哈希值的概率，从而降低了哈希冲突。
- 选择合适的素数 `a` 和字符集大小可以进一步优化哈希函数的表现。



Let's try the hash function out:

```c
hash("cat", 151, 53)

hash = (151**2 * 99 + 151**1 * 97 + 151**0 * 116) % 53
hash = (2257299 + 14647 + 116) % 53
hash = (2272062) % 53
hash = 5
```

Changing the value of `a` give us a different hash function.

```c
hash("cat", 163, 53) = 3

```

## Implementation

```c
// hash_table.c
static int ht_hash(const char* s, const int a, const int m) {
    long hash = 0;
    const int len_s = strlen(s);
    for (int i = 0; i < len_s; i++) {
        hash += (long)pow(a, len_s - (i+1)) * s[i];
        hash = hash % m;
    }
    return (int)hash;
}
```



## Pathological data

pathological: 美 [ˌpæθəˈlɑːdʒɪkl] 病理的；病理学的；病理学特有的；不正常的数据

An ideal hash function would always return an even distribution. However, for any hash function, there is a 'pathological' set of inputs, which all hash to the same value. To find this set of inputs, run a large set of inputs through the function. All inputs which hash to a particular bucket form a pathological set.

The existence of pathological input sets means there are no perfect hash functions for all inputs. The best we can do is to create a function which performs well for the expected data set.

Pathological inputs also poses a security issue. If a hash table is fed a set of colliding keys by some malicious user, then searches for those keys will take much longer (`O(n)`) than normal (`O(1)`). This can be used as a denial of service attack against systems which are underpinned by hash tables, such as DNS and certain web services.

在哈希表中，理想的哈希函数会将输入均匀分布到各个桶（bucket）中。然而，任何哈希函数都存在可能导致**哈希冲突**（collisions）的问题，即不同的输入映射到相同的哈希值。为了理解这一点，可以通过一个简单的例子来看：

### Pathological Input Sets（病态输入集）

病态输入集（pathological sets）是指一组输入，它们被哈希函数映射到相同的桶（即相同的哈希值）。这种情况通常是哈希函数设计不完善，或者输入数据具有某些特征，导致它们在哈希表中产生了冲突。

例如，如果某个哈希函数处理字符串时使用的哈希算法对相似的字符串返回相同的哈希值，或者在输入集的某些特定条件下返回相同的哈希值，那么这些输入就形成了病态输入集。在这种情况下，所有的输入将被映射到同一个桶，而不会均匀分布到哈希表的各个桶中。

### 哈希表中的查找操作

哈希表的查找时间复杂度通常是 **O(1)**，这意味着它能够快速地找到对应的值。理想情况下，哈希函数将所有键均匀地分配到不同的桶中，每个桶中的元素数量很少，因此查找的时间非常短。

然而，当多个不同的输入哈希到相同的桶时（即发生哈希冲突），哈希表通常会采用链表或其他结构来存储这些冲突的元素。对于具有多个哈希冲突的桶，查找操作需要遍历这些元素，导致时间复杂度增加，最坏情况下可能变成 **O(n)**，其中 `n` 是哈希表中元素的数量。

### 恶意攻击：拒绝服务攻击（DoS）

如果一个哈希表遭遇恶意攻击，攻击者可以故意提供一组“病态”输入，使得所有这些输入都被哈希到同一个桶中，从而引起哈希冲突。例如，攻击者可以构造一些特定的输入数据，这些数据会导致哈希函数在处理这些输入时产生哈希碰撞。通过这种方式，攻击者可以使哈希表的查找操作变得非常慢，导致系统的性能下降。

**具体的攻击方式：**

1. **恶意用户输入：** 攻击者知道哈希表使用的哈希函数，并且通过精心构造输入，使得所有输入都映射到同一个桶中。这样，哈希表在处理这些输入时需要遍历桶中的所有元素。
2. **系统性能下降：** 因为哈希表的查找操作的时间复杂度变为 **O(n)**，所以如果恶意输入数据量很大，哈希表的性能就会显著下降，导致系统的响应时间变慢。
3. **拒绝服务攻击（DoS）：** 在一些系统中，哈希表被用作核心组件，例如 DNS（域名解析系统）或某些 Web 服务。如果攻击者成功地利用哈希碰撞对哈希表进行攻击，就可以导致这些服务变得非常缓慢，甚至崩溃。这样的攻击可以使服务无法正常响应用户请求，从而造成拒绝服务的效果。

### 举个例子

假设某个Web服务使用哈希表存储用户请求的数据。当攻击者知道哈希函数的实现细节时，攻击者可以精心构造输入数据，使得所有请求都被映射到同一个桶。这样，每次处理请求时，哈希表都需要逐个检查桶中的所有项，从而导致查找时间变得非常长，系统响应时间大幅度延迟。

如果攻击者提交大量这类数据，系统的性能就会变得非常差，最终可能导致服务崩溃，造成拒绝服务攻击（Denial of Service，DoS）。

### 总结

1. **哈希冲突**：哈希表中的多个不同输入映射到相同的哈希值，导致查找变慢。
2. **病态输入集**：某些输入可能导致哈希函数返回相同的哈希值，形成病态输入集。
3. **恶意攻击**：攻击者通过提供具有特定特征的输入，导致哈希冲突，进而引起查找操作的性能下降，最终可能导致拒绝服务（DoS）攻击。

因此，设计哈希函数时，需要特别注意避免病态输入集，尽量保证哈希表的查找操作始终保持 **O(1)** 的时间复杂度，避免潜在的安全隐患。

Next section: [Handling collisions](https://github.com/jamesroutley/write-a-hash-table/blob/master/04-collisions) [Table of contents](https://github.com/jamesroutley/write-a-hash-table#contents)

