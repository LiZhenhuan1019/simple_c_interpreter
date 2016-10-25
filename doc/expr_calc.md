# `class expr_calc`
## 简介
`expr_calc`用于计算表达式的值,并执行表达式中的副作用(如修改变量的值).
`expr_calc`绑定一个符号表,用于在计算表达式和执行副作用时读写变量信息.
## Member types
member type    |definition
---------------|-----------
`relational_op`|`enum class`,表示关系运算符的种类.

## Member functions
### Constructor
* `expr_calc(symbol_table& table)`

  使构造的对象绑定到符号表`table`.

### Expression evaluation
* `int value_of(code_fragment& code)`

  使用给定的代码片段`code`来解析并计算执行code中的第一个合法表达式,返回计算得到的结果.

### Implementation
目前`expr_calc`使用*递归下降*算法来解析给出的代码,并立即计算表达式的值和执行副作用.

表达式一共分为以下几层:
1. comma 逗号表达式 右结合
  * assignment , comma
2. assignment 赋值表达式 右结合
  * var = assignment
  * relational
3. relational 关系表达式 左结合
  * relational relational_op term 其中relational_op是任意一个关系运算符
  * term
4. term + -表达式 左结合
  * term + factor
  * term - factor
  * factor
5. factor * /表达式 左结合
  * factor * prefix
  * factor / prefix
  * prefix
6. prefix 前缀自增/自减表达式 右结合
  * ++ var
  * -- var
  * postfix
7. postfix 后缀自增/自减表达式 左结合
  * var ++
  * var --
  * primary
8. primary 简单表达式 由一个变量或字面量(C语言中被称为常量)组成
  * var 变量
  * literal 字面量

该列表中表达式的优先级自上而下依次升高.
