[expr_calc]: expr_calc.md.html

# `class code_fragment`
## 简介
`code_fragment`是对一片待解析的代码的抽象.它包含一个包含代码的字符串的引用,以及一个字符串中下一个未解析的字符的下标.
## Member functions
### Constructor
* `code_fragment(std::string const&s,std::size_t begin_index = 0)`

  用一个字符串和一个起始下标构造对象,字符串表示要被解析的代码,下标表示要解析的起始位置.
  该对象将绑定到这个字符串,并存储这个起始下标.
* `code_fragment(code_fragment const&fragment,std::size_t begin_index = 0)`

  拷贝构造函数,同时可带一个下标`begin_index`,表示基于`fragment`的下标开始的要解析的起始位置.

  即:构造出的对象的起始下标 = `fragment`的起始下标 + `begin_index`

### Asignment operator
* `code_fragment& operator=(code_fragment const& lhs)`

  使`this`中的字符串引用绑定到lhs绑定的字符串,`this`的起始下标变为lhs的起始下标.

### Code access
* `char current() const`

  返回绑定的字符串中起始下标对应的字符.
* `char operator[](std::size_t pos) const`

  返回绑定的字符串中 `起始下标+pos`对应的字符.

### Code consumer
* `void eat()`

  使起始下标自增1.
## See also
* [`class expr_calc`][expr_calc]
