# Deque

## 实现细节

Deque 的基本实现要求在总项目文档和 `deque.hpp` 中已经给出，最终提交仅需要提交 `deque.hpp` 的内容。

时间复杂度要求：

- 下标随机访问：`O(1)`
- 头尾插入、删除：`O(1)`
- 迭代器访问、`iterator + n`：`O(1)`
- 随机位置插入、删除：`O(n)`

## Build & Test

在仓库根目录执行：

```bash
cmake -S deque -B build/deque
cmake --build build/deque
ctest --test-dir build/deque --output-on-failure
```

## 分数构成

课程总评中，OJ 测试部分占比：80%，code review 部分占比：20%。

3/23 18:30 截止
