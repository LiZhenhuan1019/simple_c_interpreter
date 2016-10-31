[preprocessor]:preprocessor.md.html
[expr_calc]:expr_calc.md.html
[首页](index.md.html)
# `class Simulator`

## 简介

`Simulator` 是一个用于分析并模拟运行一段C语言代码的类.

支持的C语言语法有:

类型|包括但不限于
-|-
变量声明| `int ...;`
顺序结构|`a++;b++;` <br> `{int a; {a++;}}`
选择结构|`if(a==1) a++; else a--;`
循环结构|`for(...;...;...)...`<br> `while(...)...` <br> `do...while(...)`
跳转|`break`

## 成员函数


### 构造函数

* `Simulator()`

  仅构造对象(声明内部变量),不采取任何操作.


### 析构函数

* `~Simulator()`

  释放内部变量内存空间.


### 功能函数


* `void Bind(string)`

  输入保存有一段代码的字符串.以值为0的字符结尾.  
  该字符串必须经过预处理, **代码中不能出现双引号字符串和单引号字符**.  
  函数执行完毕后,语法树构建完成,可以直接模拟运行.


* `void runSimulation(std::vector<int>&)`

  模拟运行由bind()函数分析的程序.  
  语句逐行运行, 每条语句的行号被保存为int值, 去除相邻相同值后保存在传进来的vector参数中.  
  除了以下的语句外,其它语句的行号将被丢弃:  
  + 非空且包含变量名的表达式  
  + break语句  
  + printf语句  

## See also
* [预处理器][preprocessor]
* [表达式计算][expr_calc]
