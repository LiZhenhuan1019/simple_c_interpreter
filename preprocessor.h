//
// Created by lizhenhuan on 2016/10/26.
//

#ifndef SIMPLE_C_INTERPRETER_PREPROCESSOR_H
#define SIMPLE_C_INTERPRETER_PREPROCESSOR_H

#include <string>

class preprocessor
{
public:
    preprocessor(std::string const& str)
        : code(str)
    {}
    std::string processed_code()
    {
        ensure_end_of_line();
        eliminate();
        return code;
    }
private:
    void ensure_end_of_line()
    {
        if (code.back() != '\0')
            code.push_back('\0');
    }
    void eliminate()
    {
        std::size_t cur = 0;
        while (!end_of_code(cur))
        {
            if (begin_cpp_style_coment(cur))
                cur = replace_until_end_of_line(cur);
            else if (begin_c_style_comment(cur))
                cur = replace_until_end_of_c_comment(cur);
            else if (begin_string_literal(cur))
                cur = replace_until_end_of_string_literal(cur);
            else
                ++cur;
        }
    }
    bool end_of_code(std::size_t cur)
    {
        return code[cur] == '\0';
    }
    bool begin_cpp_style_coment(std::size_t cur) const
    {
        return code[cur] == '/' && code[cur + 1] == '/';
    }
    std::size_t replace_until_end_of_line(std::size_t cur)
    {
        while (!end_of_code(cur) && code[cur] != '\n')
        {
            code[cur] = ' ';
            ++cur;
        }
        return cur + 1;
    }
    std::size_t eliminate_c_style_comment(std::size_t cur)
    {
        if (begin_c_style_comment(cur))
        {
            return replace_until_end_of_c_comment(cur + 2);
        }
        return cur;
    }
    bool begin_c_style_comment(std::size_t cur) const
    {
        return code[cur] == '/' && code[cur + 1] == '*';
    }
    bool end_of_c_comment(std::size_t cur)
    {
        return code[cur] == '*' && code[cur + 1] == '/';
    }
    std::size_t replace_until_end_of_c_comment(std::size_t cur)
    {
        code[cur] = code[cur + 1] = ' ';
        cur += 2;
        while (!end_of_code(cur) && !end_of_c_comment(cur))
        {
            if (code[cur] != '\n')//不吞跨行注释中的换行
                code[cur] = ' ';
            ++cur;
        }
        code[cur] = code[cur + 1] = ' ';
        return cur + 2;
    }
    bool begin_string_literal(std::size_t cur)
    {
        return code[cur] == '"';
    }
    bool end_of_string_literal(std::size_t cur)
    {
        return code[cur] == '"' && code[cur - 1] != '\\';
    }
    std::size_t replace_until_end_of_string_literal(std::size_t cur)
    {
        code[cur] = ' ';
        ++cur;
        while (!end_of_code(cur) && !end_of_string_literal(cur))
        {
            if (code[cur] != '\n')//不吞换行 -- 倘若添加raw string支持
                code[cur] = ' ';
            ++cur;
        }
        code[cur] = ' ';
        return cur + 1;
    }
    std::string code;
};

#endif //SIMPLE_C_INTERPRETER_PREPROCESSOR_H
