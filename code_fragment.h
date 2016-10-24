//
// Created by lizhe on 2016/10/24.
//

#ifndef SIMPLE_C_INTERPRETER_CODE_FRAGMENT_H
#define SIMPLE_C_INTERPRETER_CODE_FRAGMENT_H

#include <string>
class code_fragment
{
public:
    code_fragment(std::string const&s,std::size_t begin_index)
        :code(s),begin_index_(begin_index)
    {}
    code_fragment(code_fragment const&fragment,std::size_t begin_index)
        :code(fragment.code),begin_index_(fragment.begin_index_ + begin_index)
    {}
    char next() const
    {
        return code[begin_index_];
    }
    char operator[](std::size_t pos)
    {
        return code[begin_index_ + pos];
    }
    void eat()
    {
        begin_index_++;
    }
private:
    std::string const& code;
    std::size_t begin_index_;
};


#endif //SIMPLE_C_INTERPRETER_CODE_FRAGMENT_H
