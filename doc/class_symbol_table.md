[class_var_id]: class_var_id.md.html
[expr_calc]:expr_calc.md.html

[首页](index.md.html)

# `class symbol_table`
## 简介
`symbol_table`作为符号表的抽象,拥有添加,删除,读写变量的功能.

它可以对同一个变量同时存储多个值,但只读写最近添加的那一个值,以实现`name hiding`.

## Member functions
### Variable management
* [`var_id`][class_var_id]` add (std::string const& name)`

  向符号表中添加名为`name`的变量.变量的初始值为0(但不保证以后不会把变量初始值修改为unspecific).

  如果符号表中已有同名变量,则原有变量变为hiden变量,新的变量被称为可见变量,该名称(`name`)被称为多值名称,该变量被称为多值变量.后续所有读写操作均针对可见变量进行.
* 1). `void remove(std::string const&name)`

  2). `void remove(`[`var_id`][class_var_id]` id)`

  1. 从符号表中移除名为`name`的变量.若`name`为多值名称,则只有名为`name`的可见变量被移除,其对应的hiden变量变为可见变量.
  2. 从符号表中移除`id`指定的变量.若`id`标识的变量为多值变量,则只有`id`标识的可见变量被移除,该可见变量对应的hiden变量变为可见变量.

  若指定的变量不存在,则该函数没有其它效果.

  多值变量功能的目的是模拟C/C++中的名称隐藏(name hiding).
  如下例:
  ```
  int a = 5;
  {
      int a = 6;
      std::cout<<a;//输出6
  }
  std::cout<<a;输出5
  ```
* `bool exists(std::string const& name)`

  如果存在名为`name`的变量,则返回真,否则返回假.

### Variable access
* [`var_id`][class_var_id]` get(std::string const& name)`

  返回标识名为`name`的变量的[`var_id`][class_var_id].
* void set(std::string const& name,int value)

  若名为`name`的变量存在,则将其可见变量的值设置为`value`,否则添加名为`name`的变量并将其初始值设置为value.

## See also
* [`class var_id`][class_var_id]
* [`class expr_calc`][expr_calc]
