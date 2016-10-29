# Simple C Interpreter文档
## 简介
本项目用于解析简单的C语言代码片段,并按执行顺序输出代码的行号.目前本项目用于参加`2016"亚马逊•种子杯"编程PK赛初赛`.
## 完成度
本项目对表达式的处理的灵活程度**超过**了该比赛的要求.如,以下语句在C中合理,在本项目处理的语句中也合理(假定其中变量均经过定义):
```
a = 5;
long_name = 7;
++a +10/2 > --long_name + 4;//返回1
```
## 编译器说明
使用gcc 4.9.3或以上的编译器.并添加编译选项`-std=gnu++11 -finput-charset=utf-8 -fexec-charset=GBK`.

如果编译器支持`std::optional`,编译时可以加上选项`-DSTD_OPTIONAL`

## 组件文档
* [解释器](codesim.md.html) 用于解释执行一片代码并得出代码执行的行号序列.
* [预处理器](preprocessor.md.html) 用于对即将传给[解释器](codesim.md.html)的代码进行预处理.
* [符号表](symbol_table.md.html) 用于存储变量名及其值.
* [表达式计算](expr_calc.md.html) 用于计算表达式的值,并执行表达式中的副作用(如修改变量的值).
* [代码片段](code_fragment.md.html) 对一片待解析的代码的抽象.
