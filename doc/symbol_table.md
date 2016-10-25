# 符号表 `symbol_table`
## 简介
`symbol_table`用于存储变量名及其值.

它可以对同一个变量同时存储多个值,但只读写最近添加的那一个值,以实现`name hiding`.

## 组件
* [`class var_id`](class_var_id.md.html) 用于标识一个变量,并封装对这个变量的读写操作.
* [`class symbol_table`](class_symbol_table.md.html) 作为符号表的抽象,拥有添加,删除,读写变量的功能.
