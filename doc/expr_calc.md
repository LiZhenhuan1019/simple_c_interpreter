[class_var_id]: class_var_id.md.html

# `class expr_calc`
## 简介
`expr_calc`用于计算表达式的值,并执行表达式中的副作用(如修改变量的值).它能计算出下面的Implementation.syntax中的文法所指定的任何表达式(变量名与字面量的文法未指出,但在题目中有规定).

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
#### syntax
表达式一共分为以下几层:
1. comma 逗号表达式 右结合
  * assignment , comma
  * assignment
2. assignment 赋值表达式 右结合
  * var = assignment
  * relational
3. relational 关系表达式 左结合
  * relational relational_op term 其中relational_op是任意一个关系运算符(==,!=,<,<=,>,>=)
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

该列表中表达式对应的运算符的优先级自上而下依次升高.

#### 用到的技巧
##### 对code_fragment的使用
* 每次开始解析一个表达式时,有一点总是被保证:`code_fragment::current`返回的字符是未成功解析且将下一个被解析的字符.
* 当对`code_fragment`对象`code`进行尝试性解析时,使用一个拷贝自`code`的`code_fragment`对象`code1`,如果解析成功,执行`code = code1`,返回解析结果;否则,直接丢掉`code1`而不改变`code`,返回表示失败的标记,对`code`进行其它方式的解析.

##### 对optional的使用
* 解析失败的标记由optional负责.下面是cppreference上对std::optional的部分介绍
> The class template std::optional manages an optional contained value, i.e. a value that may or may not be present.
>
> A common use case for optional is the return value of a function that may fail. As opposed to other approaches, such as std::pair<T,bool>, optional handles expensive-to-construct objects well and is more readable, as the intent is expressed explicitly.
>
>Any instance of optional<T> at any given point in time either contains a value or does not contain a value.
>
>If an optional<T> contains a value, the value is guaranteed to be allocated as part of the optional object footprint, i.e. **no dynamic memory allocation** ever takes place. Thus, an optional object models an object, not a pointer, even though the operator*() and operator->() are defined.

鉴于optional的表达力,易用性及高效性,我决定选用optional.但由于std::optional是C++17的内容,部分编译器可能不支持,于是我在optional/my_optional.h中用std::unique_ptr写了一个简化和低效的optional.如果编译器支持且要使用高效的std::optional,在编译时定义宏STD_OPTIONAL.

对于`optional`和`code_fragment`有一点总是被保证:
若调用一个解析表达式的函数后返回的optional的值为nullopt(表示解析失败),那么传给它的code_fragment的状态与调用前的状态相同.(而若解析成功,传给它的code_fragment的状态通常要改变)
##### lvalue 和 rvalue
表达式被分为两类,lvalue和rvalue.

lvalue返回[`var_id`][class_var_id],标识变量本身,rvalue则直接返回值.

对一个层次中的左值表达式的解析通常由两个函数构成,一个尝试检测代码中是否包含该层表达式对应的运算符,另一个负责调用前一个,若检测不存在则尝试下一层表达式.

如,在赋值表达式中,lvalue_assignment尝试检测是否有赋值运算符,若有则解析赋值右侧的值(赋值运算符右侧也可以是赋值表达式,即可以连续赋值);若lvalue_assignment检测没有赋值运算符,则返回nullopt表示解析失败,即这里不应按赋值表达式的方式解析.

rvalue_assignment调用调用lvalue_assignment后检查其返回值,若不为nullopt则说明解析成功,rvalue_assignment将左值转成右值(读取变量的值)并返回;若lvalue_assignment解析失败,则rvalue_assignment调用relational进行下一层解析.
##### 对左结合表达式的解析技巧
原本`递归下降`算法只适合右结合的表达式,但这里关系运算符,算术运算符都是左结合的,不能直接用`递归下降`的方法进行解析.于是我使用了一些技巧:
一个右结合的表达式term的解析过程通常用如下伪代码表示:
```
term()
{
    value = factor()
    if(has_op())
        return value op term()//op代表运算符
    else
        return value
}
```
而如果要把它改成左结合的话,可以对它稍作修改:
```
term()
{
    value = factor()
    return right_term(value)
}
right_term(left_value)
{
    if(has_op())
        return right_term(left_value op factor()) //op代表运算符
    else return left_value
}
```
代码中的expr_calc::term/factor和expr_calc::right_term/factor几乎是上述伪代码的直接反映
