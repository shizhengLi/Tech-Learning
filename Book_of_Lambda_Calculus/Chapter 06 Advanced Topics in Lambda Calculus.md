# Chapter 6: **Advanced Topics in Lambda Calculus**

1. De Bruijn Indices
   - Replacing variable names with indices
   - Eliminating the hassles of α-conversion
   - Practical use in compilers and interpreters
2. Continuations and CPS (Continuation-Passing Style)
   - The concept of continuations
   - Modeling control flow in Lambda Calculus
   - Influence on compiler design and functional language implementations
3. Monads and Handling Side Effects
   - Simulating side effects in a purely functional model
   - Monads as a structured abstraction
   - Use cases in Haskell and other functional languages
4. Applications in Compiler Design
   - Translating high-level languages into a lambda-based IR
   - Optimizations and equivalence transformations
   - Real-world compiler examples that utilize lambda-based representations
5. Reduction and Rewriting Systems
   - The idea of rewriting systems
   - Normalization strategies
   - Optimizations for more complex expressions

# 第6章　**Lambda Calculus的高级主题**

1. De Bruijn索引
   - 用索引替代变量命名
   - 避免α-变换的麻烦
   - 编译器和解释器中的应用
2. Continuation与CPS转换
   - Continuation的概念
   - 在Lambda Calculus中模拟控制流
   - 对编译器和函数式语言实现的影响
3. Monad与副作用
   - 在纯Lambda Calculus中模拟副作用
   - Monad作为一种结构化的抽象
   - 与Haskell等语言中的使用实例
4. 在编译器设计中的应用
   - 将高级语言编译为Lambda中间表示
   - 优化与等价变换
   - 实际编译器案例分析（部分编译器会使用类Lambda IR）
5. 归约与重写系统
   - 归约系统（Rewriting System）的概念
   - 正规化（Normalization）的思路
   - 复杂表达式优化策略