# Simple C Interpreter文档
## 简介
本项目用于解析简单的C语言代码片段,并按执行顺序输出代码的行号.目前本项目用于参加`2016"亚马逊•种子杯"编程PK赛初赛`.
## 编译器说明
使用gcc 4.9.3或以上的编译器.
如果编译器支持std::optional,编译时可以加上选项-DSTD_OPTIONAL

## 组件文档
* [符号表](symbol_table.md.html) 用于存储变量名及其值.
* [表达式计算](expr_calc.md.html) 用于计算表达式的值,并执行表达式中的副作用(如修改变量的值).
* [代码片段](code_fragment.md.html) 对一片待解析的代码的抽象.
