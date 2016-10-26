//
// Created by lizhe on 2016/10/26.
//

#ifndef SIMPLE_C_INTERPRETER_INTERPRETER_H
#define SIMPLE_C_INTERPRETER_INTERPRETER_H
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
class code_controller
{
public:
    struct position
    {
        std::size_t line_num;
        std::size_t column_num;
    };

    code_controller(std::string const&code)
        :pos({0,0})
    {
        std::size_t begin_pos = 0;
        std::size_t newline_pos = code.find('\n',begin_pos);
        auto str = code.substr(begin_pos,newline_pos - begin_pos);
        std::cout<<str;
        codes.push_back(str);
        while(newline_pos != code.npos)
        {
            begin_pos = newline_pos + 1;
            newline_pos = code.find('\n',begin_pos);
            str = code.substr(begin_pos,newline_pos-begin_pos);
            std::cout << str;
            codes.push_back(str);
        }
        std::cout<<"constructor end \n";
    }
    char current()
    {
        return codes[pos.line_num][pos.column_num];
    }
    void eat()
    {
        if (reach_end_of_line())
            if (reach_last_line())
                confirm_reach_end();
            else
                go_to_next_line();
        else
            go_to_next_char();
    }
    char current_and_eat()
    {
        char c = current();
        eat();
        return c;
    }
    bool reach_end()
    {
        return pos.column_num == codes[pos.line_num].size();
    }
    void skip_space()//TODO: incomplete
    {
        while (1)
            if(is_space())
                eat();
            else if(is_comment())
                skip_comment();
            else
                break;
    }

    position& current_pos()
    {
        return pos;
    }
    position const& current_pos() const
    {
        return pos;
    }
private:
    bool reach_end_of_line() const
    {
        return pos.column_num >= codes[pos.line_num].size() - 1;
    }
    bool reach_last_line() const
    {
        return pos.line_num == codes.size() - 1;
    }
    bool is_space() const
    {
        return c == ' ' || c == '\n' || c == '\t';
    }
    bool is_comment()
    {
        auto pre_pos = current_pos();
        if(current_and_eat() == '/' && current() == '*')

    }
    void confirm_reach_end()
    {
        if(pos.column_num == codes[pos.line_num].size() - 1)
            pos.column_num = codes[pos.line_num].size();
    }
    void go_to_next_line()
    {
        ++pos.line_num;
        pos.column_num = 0;
    }
    void go_to_next_char()
    {
        ++pos.column_num;
    }
    std::vector<std::string> codes;
    position pos;
};

class interpreter
{

};

#endif //SIMPLE_C_INTERPRETER_INTERPRETER_H
