//
// Created by lizhe on 2016/10/24.
//

#ifndef SIMPLE_C_INTERPRETER_EXPRESSION_INTERPRETER_H
#define SIMPLE_C_INTERPRETER_EXPRESSION_INTERPRETER_H

#include "code_fragment.h"
#include "symbol_table.h"
#include <experimental/optional>

class symbol_table;

using std::experimental::optional;
using std::experimental::nullopt;

class expr_calc
{
    enum class relational_op
    {
        equal, not_equal, less, less_or_equal, greater, greater_or_equal
    };
public:
    expr_calc(symbol_table& table)
        : table_(table)
    {

    }
    int value_of(code_fragment& code)
    {
        return rvalue_assignment(code);
    }
private:
    int rvalue_assignment(code_fragment& code)
    {
        if (auto lvalue = lvalue_assignment(code)) //declaration as condition: test if lvalue is non-zero
        {
            return lvalue.value().value();
        }
        else
            return relational(code);
    }
    optional<var_id> lvalue_assignment(code_fragment& code)
    {
        code_fragment& lvalue_fragment(code);
        auto lvalue = lvalue_prefix(lvalue_fragment);
        if (lvalue)
            if (skipping_space_get_current_and_eat(lvalue_fragment) == '=')
            {
                if(lvalue_fragment.current() != '=') //if it is not equality operator
                {
                    code = lvalue_fragment;
                    int rvalue = rvalue_assignment(code);
                    lvalue.value().set(rvalue);
                    return lvalue;
                }
            }
        return nullopt;
    }
    int relational(code_fragment& code)
    {
        int left = term(code);
        code_fragment relational_fragment(code);
        auto opt = relational_op_type(relational_fragment);
        if(opt)
        {
            relational_op op_type = opt.value();
            code = relational_fragment;
            int right = term(code);
            switch (op_type)
            {
            case relational_op::equal:
                return left == right;
            case relational_op::not_equal:
                return left != right;
            case relational_op::less_or_equal:
                return left <= right;
            case relational_op::less:
                return left < right;
            case relational_op::greater_or_equal:
                return left >= right;
            case relational_op::greater:
                return left > right;
            }
        }
        else
            return left;
    }
    optional<relational_op> relational_op_type(code_fragment& code)
    {
        char c = skipping_space_get_current_and_eat(code);
        if (c == '=')
            if (code.current_and_eat() == '=')
                return relational_op::equal;
            else
                return nullopt;
        else if (c == '!')
            if (code.current_and_eat() == '=')
                return relational_op::not_equal;
            else
                throw std::string("Interpret failed!Expect '!=' operator.In ") + __func__;
        else if (c == '<')
            if (code.current_and_eat() == '=')
                return relational_op::less_or_equal;
            else
                return relational_op::less;
        else if (c == '>')
            if (code.current_and_eat() == '=')
                return relational_op::greater_or_equal;
            else
                return relational_op::greater;
        return nullopt;
    }
    int term(code_fragment& code)
    {
        int left = factor(code);
        return right_term(left,code);
    }
    int right_term(int left_value, code_fragment& code)
    {
        code_fragment term_fragment(code);
        char c = skipping_space_get_current_and_eat(term_fragment);
        if(c == '+')
        {
            code = term_fragment;
            return right_term(left_value + factor(code),code);
        }
        else if(c == '-')
        {
            code = term_fragment;
            return right_term(left_value - factor(code),code);
        }
        else
            return left_value;
    }
    int factor(code_fragment& code)
    {
        int left = rvalue_prefix(code);
        return right_factor(left,code);
    }
    int right_factor(int left_value, code_fragment& code)
    {
        code_fragment factor_fragment(code);
        char op = skipping_space_get_current_and_eat(factor_fragment);
        if(op == '*')
        {
            code = factor_fragment;
            return right_factor(left_value * rvalue_prefix(code),code);
        }
        else if( op == '/')
        {
            code = factor_fragment;
            return right_factor(left_value / rvalue_prefix(code),code);
        }
        else
            return left_value;
    }
    int rvalue_prefix(code_fragment& code)
    {
        return literal(code);
    }
    optional<var_id> lvalue_prefix(code_fragment& code)
    {
        return lvalue_var(code);
    }
    int postfix(code_fragment& code)
    {
        return literal(code);
    }
    int rvalue_var(code_fragment& code)
    {
        return literal(code);
    }
    optional<var_id> lvalue_var(code_fragment& code)
    {
        return nullopt;
    }
    int literal(code_fragment& code)
    {
        return read_literal(code).value();
    }

    bool is_space(char c)
    {
        return c == ' ' || c == '\n' || c == '\t';
    }
    void skip_space(code_fragment& code)
    {
        char c = code.current();
        while (is_space(c))
        {
            code.eat();
            c = code.current();
        }
    }
    char skipping_space_get_current_and_eat(code_fragment& code) //跳过空格,返回空格后的第一个字符,并再跳过一个字符.
    {
        skip_space(code);
        char c = code.current();
        code.eat();
        return c;
    }
    optional<std::string> read_identifier(code_fragment& code)
    {
        code_fragment& id_fragment(code);
        skip_space(id_fragment);
        char c = id_fragment.current_and_eat();
        if(std::isalpha(c))
        {
            std::string name;
            name.push_back(c);
            while(c = id_fragment.current(),std::isalnum(c))
            {
                id_fragment.eat();
                name.push_back(c);
            }
            code = id_fragment;
            return name;
        }
        return nullopt;
    }
    optional<int> read_literal(code_fragment& code)
    {
        code_fragment& literal_fragment(code);
        skip_space(literal_fragment);
        char c = literal_fragment.current_and_eat();
        if(std::isdigit(c))
        {
            std::string digits;
            digits.push_back(c);
            while(c = literal_fragment.current(),std::isdigit(c))
            {
                literal_fragment.eat();
                digits.push_back(c);
            }
            code = literal_fragment;
            return std::stoi(digits);
        }
        return nullopt;
    }
    symbol_table& table_;
};


#endif //SIMPLE_C_INTERPRETER_EXPRESSION_INTERPRETER_H
