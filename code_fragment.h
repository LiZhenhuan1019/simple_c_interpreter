//
// Created by lizhe on 2016/10/24.
//

#ifndef SIMPLE_C_INTERPRETER_CODE_FRAGMENT_H
#define SIMPLE_C_INTERPRETER_CODE_FRAGMENT_H

#include <string>
#include <functional>
class code_fragment
{
public:
    code_fragment(std::string const&s,std::size_t begin_index = 0)
        :code(s),begin_index_(begin_index)
    {}
    code_fragment(code_fragment const&fragment,std::size_t begin_index = 0)
        :code(fragment.code),begin_index_(fragment.begin_index_ + begin_index)
    {}
    code_fragment& operator=(code_fragment const& lhs)
    {
        code = lhs.code;
        begin_index_ = lhs.begin_index_;
        return *this;
    }
    char current() const
    {
        return code.get()[begin_index_];
    }
    char current_and_eat()
    {
        char c = current();
        eat();
        return c;
    }
    char operator[](std::size_t pos) const
    {
        return code.get()[begin_index_ + pos];
    }
    void eat()
    {
        begin_index_++;
    }
private:
    std::reference_wrapper<const std::string> code;
    std::size_t begin_index_;
};


#endif //SIMPLE_C_INTERPRETER_CODE_FRAGMENT_H
