[codesim]:codesim.md.html
[首页](index.md.html)
# `class preprocessor`
## 简介
`preprocessor`用于对即将传给[解释器][codesim]的"代码"进行预处理.

它将"代码"中的所有注释和字符串替换为空格,同时保留其中的换行符.

同时它确保"代码"的结尾有一个'\0'字符.

预处理后的"代码"将被传给[解释器][codesim]进行解释执行.
## Member functions
### Constructor
* `preprocessor(std::string const& str)`

  用字符串`str`构造`preprocessor`对象,使其准备对`str`进行预处理.

### Preprocess
* `std::string processed_code()`

  对构造时指定的字符串进行预处理并返回存储预处理结果的字符串.

## See also
[解释器][codesim]
