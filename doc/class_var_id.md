[class_symbol_table]:class_symbol_table.md.html
[expr_calc]:expr_calc.md.html

# `class var_id`
## 简介
`var_id`用于标识一个变量,并封装对这个变量的读写操作.
## Member types
member type |definition
------------|----------
`iterator_t`| `std::map<std::string,std::vector<int>>::iterator`
## Member functions
### Constructor
* `var_id(iterator_t iter)`

  用给定的迭代器`iter`构造`var_id`,`iter`必须指向存在的元素.该`var_id`对象将标识`iter`指向的元素代表的变量.

### Read
* `std::string const& name() const`

  返回标识的变量的名字.

* `int value() const`

  返回标识的变量的值.

### Write
* `void set(int value)`

  将标识的变量的值设置为`value`.

## See also
* [`class symbol_table`][class_symbol_table]
* [`class expr_calc`][expr_calc]
