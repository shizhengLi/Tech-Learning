# Methods



Link: [https://github.com/jamesroutley/write-a-hash-table/tree/master/05-methods](https://github.com/jamesroutley/write-a-hash-table/tree/master/05-methods)

Our hash function will implement the following API:

```c
// hash_table.h
void ht_insert(ht_hash_table* ht, const char* key, const char* value);
char* ht_search(ht_hash_table* ht, const char* key);
void ht_delete(ht_hash_table* h, const char* key);
```

## Insert

To insert a new key-value pair, we iterate through indexes until we find an empty bucket. We then insert the item into that bucket and increment the hash table's `count` attribute, to indicate a new item has been added. A hash table's `count` attribute will become useful when we look at [resizing](https://github.com/jamesroutley/write-a-hash-table/blob/master/06-resizing) in the next section.

```c
// hash_table.c
void ht_insert(ht_hash_table* ht, const char* key, const char* value) {
    ht_item* item = ht_new_item(key, value);
    int index = ht_get_hash(item->key, ht->size, 0);
    ht_item* cur_item = ht->items[index];
    int i = 1;
    while (cur_item != NULL) {
        index = ht_get_hash(item->key, ht->size, i);
        cur_item = ht->items[index];
        i++;
    } 
    ht->items[index] = item;
    ht->count++;
}
```

### 这段代码的作用

这段代码实现了向哈希表中插入一个新的键值对的功能。具体步骤如下：

1. **创建新项**：
    通过 `ht_new_item` 函数创建一个新的哈希表项（`ht_item`）。每个哈希表项包含一个键（`key`）和一个值（`value`）。

   ```c
   ht_item* item = ht_new_item(key, value);
   ```

2. **计算初始哈希值**：
    使用 `ht_get_hash` 函数计算这个键对应的哈希表索引。初始的探查次数是 `0`。

   ```c
   int index = ht_get_hash(item->key, ht->size, 0);
   ```

3. **处理冲突（开放寻址法）**：
    检查 `index` 位置是否已经有元素。如果该位置已经被占用（`cur_item != NULL`），则发生了冲突，需要使用**双重哈希**重新计算下一个位置。

   - 通过增加 `i`，即探查次数，调用 `ht_get_hash` 来重新计算哈希值，继续查找下一个桶的位置。
   - 这个过程会重复进行，直到找到一个空桶为止。

   ```c
   ht_item* cur_item = ht->items[index];
   int i = 1;
   while (cur_item != NULL) {
       index = ht_get_hash(item->key, ht->size, i);
       cur_item = ht->items[index];
       i++;
   }
   ```

4. **插入新项**：
    找到空桶后，插入新项（`item`）到哈希表的相应位置。

   ```c
   ht->items[index] = item;
   ```

5. **更新计数器**：
    增加哈希表中的元素计数器 `count`，表示成功插入了一个新项。

   ```c
   ht->count++;
   ```

### 边界或稀有情况说明

1. **哈希表为空时插入**：
    当哈希表是空的或者所有桶都没有占用时，`ht_get_hash` 会直接计算出一个空桶的索引，`cur_item` 为 `NULL`，进入插入操作。这是最常见的情况。
   - 例如，哈希表大小为 10，插入 `key = "apple"`，假设 `ht_get_hash("apple", 10, 0)` 计算结果为 3，则 `index = 3`，如果 `ht->items[3] == NULL`，就直接插入 `item` 到 `ht->items[3]`。
2. **哈希冲突时的处理**：
    如果存在哈希冲突，即多个不同的键计算出的哈希值相同，那么我们会通过双重哈希技术找到下一个可用的桶位置。
   - 假设哈希表大小为 10，插入两个不同的键 `key = "apple"` 和 `key = "banana"`，且两个键通过哈希函数 `ht_get_hash` 计算出的初始索引都为 3。那么，当插入 `key = "banana"` 时，`ht->items[3]` 会被占用，接着计算下一个位置。
   - 例如，`ht_get_hash("banana", 10, 1)` 可能返回索引 4，这时 `index = 4`，`ht->items[4]` 是空的，可以将 `"banana"` 插入到位置 4。
3. **探查次数超过哈希表大小时的处理**：
    如果哈希表的大小非常小，或者哈希函数的设计不佳，可能会发生很多冲突。即使进行多次探查，也可能找不到空桶，导致探查次数超过了哈希表的大小 `num_buckets`。
   - 这是一个稀有情况，通常需要重新调整哈希表的大小，以避免这种情况的发生（通常称为哈希表**扩容**）。
   - 在当前代码中没有做扩容处理，但是实际应用中，这种情况会在 `count` 达到一定阈值时触发，重新调整哈希表的大小并重新哈希所有元素。
4. **所有桶都被占满**：
    如果哈希表中的所有桶都已经被占用，且插入新的项时无法通过开放寻址找到空桶（即所有位置都已经被占用且计算出来的索引都已经填满），那么 `ht_insert` 函数就会陷入死循环，因为它会不断探查直到越界。
   - 为了避免这种情况，哈希表通常会根据元素的数量与桶的数量的比例，设定一个负载因子阈值，当哈希表的负载因子达到一定值时，进行扩容操作。

### 例子演示

假设哈希表的大小为 5，插入以下两个键值对：

1. 插入 `"apple" => "fruit"`
   - `ht_get_hash("apple", 5, 0)` 计算出索引 `3`，位置为空，直接插入。
2. 插入 `"banana" => "fruit"`
   - `ht_get_hash("banana", 5, 0)` 计算出索引 `3`，但是 `ht->items[3]` 已经有 `"apple"`。
   - 计算第二次探查位置：`ht_get_hash("banana", 5, 1)` 计算出索引 `4`，位置为空，插入 `"banana"`。
3. 插入 `"cherry" => "fruit"`
   - `ht_get_hash("cherry", 5, 0)` 计算出索引 `2`，位置为空，直接插入。

最终的哈希表内容为：

```c
Index 0: NULL
Index 1: NULL
Index 2: cherry => fruit
Index 3: apple => fruit
Index 4: banana => fruit
```

### 总结

- 这段代码通过开放寻址法和双重哈希来处理哈希冲突，能够在哈希表中插入新的键值对。
- 如果哈希表已满或发生过多冲突，可能导致探查效率下降甚至死循环，因此通常需要根据负载因子调整哈希表的大小，进行**扩容**。
- 需要注意的边界情况包括哈希表为空、哈希冲突的处理、探查次数过多、以及所有桶都被占满的情况。



## Search

Searching is similar to inserting, but at each iteration of the `while` loop, we check whether the item's key matches the key we're searching for. If it does, we return the item's value. If the while loop hits a `NULL` bucket, we return `NULL`, to indicate that no value was found.



```c
// hash_table.c
char* ht_search(ht_hash_table* ht, const char* key) {
    int index = ht_get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;
    while (item != NULL) {
        if (strcmp(item->key, key) == 0) {
            return item->value;
        }
        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    } 
    return NULL;
}
```

### `ht_search` 函数介绍

`ht_search` 函数用于在哈希表中查找给定键 `key` 对应的值。如果找到了相应的键值对，就返回该键对应的值；如果未找到，则返回 `NULL`。

#### 函数步骤：

1. **计算初始哈希值**： 首先，通过 `ht_get_hash` 函数计算给定键 `key` 在哈希表中的初始索引。

   ```c
   int index = ht_get_hash(key, ht->size, 0);
   ```

2. **检查该位置的元素**： 在计算出的 `index` 位置检查是否存在元素。如果该位置有元素，就进行下一步的比较。

   ```c
   ht_item* item = ht->items[index];
   ```

3. **遍历处理冲突**： 如果该位置的元素与查找的键 `key` 不匹配，就进入 `while` 循环，继续查找下一个可能的桶位置。使用双重哈希来处理哈希冲突。每次碰到冲突时，增加探查次数 `i`，重新计算下一个桶的位置。

   ```c
   while (item != NULL) {
       if (strcmp(item->key, key) == 0) {
           return item->value;
       }
       index = ht_get_hash(key, ht->size, i);
       item = ht->items[index];
       i++;
   }
   ```

4. **键匹配**： 如果当前位置的元素的 `key` 与查找的 `key` 相同，返回该元素的 `value`。这里使用 `strcmp` 函数来比较两个字符串是否相等。

   ```c
   if (strcmp(item->key, key) == 0) {
       return item->value;
   }
   ```

5. **返回 `NULL`**： 如果遍历完所有可能的桶位置后仍然没有找到匹配的键，说明哈希表中不存在该键，返回 `NULL`。

   ```c
   return NULL;
   ```

### `strcmp` 函数介绍

`strcmp` 是一个标准的 C 函数，用于比较两个字符串是否相等。

- **函数原型**：

  ```c
  int strcmp(const char* str1, const char* str2);
  ```

- **功能**： `strcmp` 比较两个字符串 `str1` 和 `str2`，返回值根据比较结果不同而不同：

  - 返回 `0`：表示 `str1` 和 `str2` 相等。
  - 返回负值：表示 `str1` 小于 `str2`（按字典顺序比较）。
  - 返回正值：表示 `str1` 大于 `str2`（按字典顺序比较）。

- **具体用途**： 在哈希表的查找中，`strcmp` 用来比较当前哈希桶中的 `key` 和目标 `key` 是否匹配。如果返回值为 `0`，说明两个字符串相同，此时返回当前项的 `value`。如果不相同，就继续查找下一个桶。

  ```c
  if (strcmp(item->key, key) == 0) {
      return item->value;
  }
  ```

### 例子演示

假设有一个哈希表 `ht`，其中包含以下键值对：

- `"apple" => "fruit"`
- `"banana" => "fruit"`
- `"cherry" => "fruit"`

并且 `ht_search` 函数被用来查找 `key = "banana"` 对应的值。

1. 首先，通过哈希函数计算出 `key = "banana"` 对应的初始哈希值。
2. 在哈希表中查找对应位置的元素。假设 `ht->items[3]` 是 `"apple" => "fruit"`，则与目标键 `"banana"` 不匹配。
3. 继续计算下一个位置，可能通过双重哈希得到了索引 4。
4. 在位置 4 找到 `"banana" => "fruit"`，使用 `strcmp` 比较键，发现它们相等，返回 `"fruit"`。

### 总结

- `ht_search` 函数通过哈希查找和冲突解决，找到并返回给定键 `key` 对应的值。
- `strcmp` 用于字符串比较，帮助判断哈希表中的键是否与目标键相同。

## Delete

Deleting from an open addressed hash table is more complicated than inserting or searching. The item we wish to delete may be part of a collision chain. Removing it from the table will break that chain, and will make finding items in the tail of the chain impossible. To solve this, instead of deleting the item, we simply mark it as deleted.

We mark an item as deleted by replacing it with a pointer to a global sentinel item which represents that a bucket contains a deleted item.

```c
// hash_table.c
static ht_item HT_DELETED_ITEM = {NULL, NULL};


void ht_delete(ht_hash_table* ht, const char* key) {
    int index = ht_get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;
    while (item != NULL) {
        if (item != &HT_DELETED_ITEM) {
            if (strcmp(item->key, key) == 0) {
                ht_del_item(item);
                ht->items[index] = &HT_DELETED_ITEM;
            }
        }
        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    } 
    ht->count--;
}
```



### 代码解析

#### 1. **`HT_DELETED_ITEM` 的定义**：

```c
static ht_item HT_DELETED_ITEM = {NULL, NULL};
```

- 这里定义了一个全局的标志项 `HT_DELETED_ITEM`，它作为一个哨兵项用于标记哈希表中的已删除项。
- `HT_DELETED_ITEM` 的 `key` 和 `value` 都是 `NULL`，它的唯一作用是表示某个桶曾经被删除过。因为直接删除项会破坏冲突链，所以使用哨兵项可以避免这个问题。

#### 2. **`ht_delete` 函数的目的**：

`ht_delete` 函数的作用是删除哈希表中的一个键值对。如果找到目标键，则将对应的桶标记为已删除，而不是直接删除它。

#### 3. **`ht_delete` 的实现**：

```c
void ht_delete(ht_hash_table* ht, const char* key) {
    int index = ht_get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;
    while (item != NULL) {
        if (item != &HT_DELETED_ITEM) {
            if (strcmp(item->key, key) == 0) {
                ht_del_item(item);
                ht->items[index] = &HT_DELETED_ITEM;
            }
        }
        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    } 
    ht->count--;
}
```

#### 主要步骤：

1. **计算初始哈希值**：

   ```c
   int index = ht_get_hash(key, ht->size, 0);
   ```

   通过 `ht_get_hash` 函数计算 `key` 对应的初始桶位置。

2. **查找目标键值对**：

   ```c
   ht_item* item = ht->items[index];
   ```

   获取当前桶位置的元素 `item`。

3. **处理冲突**： 如果当前位置的元素是 `NULL` 或已经是已删除项（`HT_DELETED_ITEM`），则停止继续查找。 否则，比较桶中的元素 `key` 和目标 `key` 是否相等。

4. **删除目标项**：

   ```c
   if (strcmp(item->key, key) == 0) {
       ht_del_item(item);
       ht->items[index] = &HT_DELETED_ITEM;
   }
   ```

   - 如果找到了匹配的 `key`，调用 `ht_del_item` 删除该元素（释放内存）。
   - 将哈希表中对应的桶（`ht->items[index]`）标记为已删除项，即指向 `HT_DELETED_ITEM`。

5. **继续查找下一个桶**：

   ```c
   index = ht_get_hash(key, ht->size, i);
   item = ht->items[index];
   i++;
   ```

   如果当前桶位置的项不是目标项，继续使用双重哈希计算下一个桶位置，增加 `i` 进行多次尝试查找下一个可能的桶。

6. **减少项计数**：

   ```c
   ht->count--;
   ```

   如果成功删除了一个项，将哈希表中的 `count` 减 1，表示当前哈希表的项数减少了。

#### 4. **标记为已删除而不是直接删除**：

- 由于哈希表使用开放寻址法（open addressing），删除一个元素会破坏后续元素的查找，因为哈希冲突时，后续的元素可能在当前元素之后。
- 通过使用 `HT_DELETED_ITEM` 作为哨兵，我们可以确保在链中保持元素的顺序。即使该位置的元素被删除了，后续的元素仍然可以通过哈希表找到。

#### 5. **为什么不直接删除项？**

- **防止破坏链**：如果直接删除项，后续的元素（如果它们使用该位置进行冲突处理）会无法找到，导致查找失败。标记为删除项可以保证这些元素仍然可以被正确找到。
- **删除后的处理**：当一个桶被标记为已删除时，虽然该桶不再存储有效的键值对，但它仍然可以继续参与冲突处理。后续的元素可以在此位置被插入。

### 示例说明

假设哈希表的大小为 5，存在以下键值对：

| 索引 | 键       | 值      |
| ---- | -------- | ------- |
| 0    | "apple"  | "fruit" |
| 1    | "banana" | "fruit" |
| 2    | "cherry" | "fruit" |

并且 `ht_delete(ht, "banana")` 被调用。删除过程如下：

1. 计算 `"banana"` 的哈希值并找到对应的桶位置（假设为索引 1）。
2. 在索引 1 位置找到 `"banana"`，匹配成功，进行删除。
3. 删除 `"banana"` 后，不直接释放内存，而是将该桶位置标记为已删除，即设置为 `&HT_DELETED_ITEM`。
4. 此时，哈希表如下所示：

| 索引 | 键       | 值       |
| ---- | -------- | -------- |
| 0    | "apple"  | "fruit"  |
| 1    | (已删除) | (已删除) |
| 2    | "cherry" | "fruit"  |

虽然 `"banana"` 已经被删除，但因为索引 1 仍然保存了 `HT_DELETED_ITEM`，所以如果插入新的元素，哈希表会继续考虑这个位置。

### 总结

- **标记删除**：通过标记已删除项而不是直接删除，可以保持哈希表的完整性，避免冲突链断裂。
- **冲突处理**：在开放寻址法中，删除操作需要特别处理，以避免影响后续的查找操作。
- **减少计数**：删除项后，记得减少哈希表中的 `count`，确保表的大小反映实际存储的项数。





After deleting, we decrement the hash table's `count` attribute.

We also need to modify `ht_insert` and `ht_search` functions to take account of deleted nodes.

When searching, we ignore and 'jump over' deleted nodes. When inserting, if we hit a deleted node, we can insert the new node into the deleted slot.



```c
// hash_table.c
void ht_insert(ht_hash_table* ht, const char* key, const char* value) {
    // ...
    while (cur_item != NULL && cur_item != &HT_DELETED_ITEM) {
        // ...
    }
    // ...
}


char* ht_search(ht_hash_table* ht, const char* key) {
    // ...
    while (item != NULL) {
        if (item != &HT_DELETED_ITEM) { 
            if (strcmp(item->key, key) == 0) {
                return item->value;
            }
        }
        // ...
    }
    // ...
}
```

## Update

Our hash table doesn't currently support updating a key's value. If we insert two items with the same key, the keys will collide, and the second item will be inserted into the next available bucket. When searching for the key, the original key will always be found, and we are unable to access the second item.

We can fix this my modifying `ht_insert` to delete the previous item and insert the new item at its location.

```c
// hash_table.c
void ht_insert(ht_hash_table* ht, const char* key, const char* value) {
    // ...
    while (cur_item != NULL) {
        if (cur_item != &HT_DELETED_ITEM) {
            if (strcmp(cur_item->key, key) == 0) {
                ht_del_item(cur_item);
                ht->items[index] = item;
                return;
            }
        }
        // ...
    } 
    // ...
}
```

这段代码的目的是为了**更新哈希表中已经存在的键**。具体来说，它处理了如果插入一个新项的键和哈希表中已有项的键相同的情况。在这种情况下，哈希表应该删除原有项，并将新项插入到相同的位置，从而实现键值的更新。

### 详细解析：

#### 1. **查找是否存在相同的键**：

```c
while (cur_item != NULL) {
    if (cur_item != &HT_DELETED_ITEM) {
        if (strcmp(cur_item->key, key) == 0) {
            ht_del_item(cur_item);
            ht->items[index] = item;
            return;
        }
    }
    // ...
}
```

- **`cur_item != NULL`**：表示当前桶内有元素，如果桶为空，说明该桶没有任何元素。
- **`cur_item != &HT_DELETED_ITEM`**：这一步确保当前桶中存储的不是已删除的哨兵项（`HT_DELETED_ITEM`），如果是已删除的项，则跳过该桶。
- **`strcmp(cur_item->key, key) == 0`**：比较当前元素的 `key` 和传入的 `key` 是否相同。如果相同，意味着我们找到了一个键值相同的项，可以进行更新操作。

#### 2. **删除原有项并插入新项**：

```c
ht_del_item(cur_item);
ht->items[index] = item;
return;
```

- **`ht_del_item(cur_item)`**：删除原来的项，释放其内存。这里会调用 `ht_del_item` 来清理原有项的内存（包括 `key` 和 `value`）。
- **`ht->items[index] = item`**：将新项插入到原先项所在的位置（`index`）。
- **`return`**：插入新项并完成更新后，直接返回，不再继续执行后面的代码。

#### 3. **处理冲突**：

如果哈希表中存在冲突，程序会继续尝试查找下一个位置。只有当找到相同的 `key` 时才会执行更新操作。

### 例子说明：

假设哈希表的大小为 5，且已有以下键值对：

| 索引 | 键       | 值      |
| ---- | -------- | ------- |
| 0    | "apple"  | "fruit" |
| 1    | "banana" | "fruit" |
| 2    | "cherry" | "fruit" |

现在，如果插入一个新的项 `("banana", "yellow fruit")`，由于 `"banana"` 已经存在，我们希望更新 `"banana"` 对应的值。

1. 计算 `"banana"` 的哈希值并找到对应的桶位置（假设是索引 1）。
2. 在索引 1 位置找到 "banana"，然后：
   - 删除 `"banana"` 对应的项。
   - 在索引 1 位置插入新的项 `("banana", "yellow fruit")`。

此时，哈希表变为：

| 索引 | 键       | 值             |
| ---- | -------- | -------------- |
| 0    | "apple"  | "fruit"        |
| 1    | "banana" | "yellow fruit" |
| 2    | "cherry" | "fruit"        |

### 总结：

- **更新操作**：通过检查哈希表中的键是否已经存在，如果存在则删除原项并插入新项，从而实现更新功能。
- **哈希表性能**：这个方法避免了在插入相同键时发生的插入冲突，并保持了哈希表的结构不变。



Next section: [Resizing tables](https://github.com/jamesroutley/write-a-hash-table/blob/master/06-resizing) [Table of contents](https://github.com/jamesroutley/write-a-hash-table#contents)



