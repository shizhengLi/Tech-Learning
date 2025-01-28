### Chapter 5: Building a Simple Lambda Calculus Interpreter

1. Architecture of an Interpreter
   - Parser and abstract syntax tree (AST)
   - Overall structure of an interpreter or compiler
2. Lexical and Syntactic Analysis
   - Using a simple BNF/EBNF grammar for lambda expressions
   - Designing a basic parser
3. Evaluation Strategies
   - Normal-order vs. applicative-order evaluation
   - Lazy vs. strict evaluation
   - Example: The same lambda expression under different strategies
4. Implementation in C or Python
   - Data structures (C structs or Python classes)
   - Recursive evaluation and variable substitution
   - Detailed code listings with explanations
5. Testing and Debugging
   - Example test cases (booleans, arithmetic, recursion)
   - Common errors and how to troubleshoot them

### 第5章　**实现一个简单的Lambda Calculus解释器**

1. 解释器原理与架构
   - 解析器（Parser）与抽象语法树（AST）
   - 解释器或编译器的总体结构
2. 词法与语法分析
   - 用简单的BNF或EBNF定义Lambda表达式的语法
   - 设计一个简易的Parser
3. 求值策略
   - 评估策略：正常序（Normal Order）与应用序（Applicative Order）
   - 惰性求值与严格求值的影响
   - 小节示例：同一个表达式在不同策略下的求值过程
4. 用C或Python实现
   - 基础数据结构定义（C语言中的结构体或Python类）
   - 递归求值与变量替换逻辑
   - 代码示例+详细注释
5. 测试与调试
   - 测试用例：布尔运算、数值运算、递归函数
   - 可能遇到的错误及调试方法