//
// Created by lizhe on 2016/10/24.
//

#ifndef SIMPLE_C_INTERPRETER_EXPRESSION_INTERPRETER_H
#define SIMPLE_C_INTERPRETER_EXPRESSION_INTERPRETER_H

#include "code_fragment.h"
#include "symbol_table.h"
#include <stdexcept>
#ifdef STD_OPTIONAL
#include <experimental/optional>
using std::experimental::optional;
using std::experimental::nullopt;
#else
#include "optional/my_optional.h"
#endif


class expr_calc
{
    enum class relational_op
    {
        equal, not_equal, less, less_or_equal, greater, greater_or_equal
    };
public:
    expr_calc(symbol_table& table,std::string const& code)//如果code的实参是temporary,构造的expr_calc也应是temporary
        : table_(table),code_(code)
    {

    }
    int value_of_expr(std::size_t begin_pos = 0) //可为空表达式
    {
        code_fragment code(code_,begin_pos);
        return comma(code);
    }
    int value_of_initializer(std::size_t begin_pos = 0) //不解析',',不能为空表达式
    {
        code_fragment code(code_,begin_pos);
        return rvalue_assignment(code);
    }
private:
    int comma(code_fragment& code)
    {
        int left = rvalue_assignment(code);
        code_fragment comma_fragment(code);
        if(skipping_space_get_current_and_eat(comma_fragment) == ',')
        {
            code = comma_fragment;
            return comma(code);
        }
        else
            return left;
    }
    int rvalue_assignment(code_fragment& code)
    {
        if(empty_expr(code))
            return 0;
        if (auto lvalue = lvalue_assignment(code)) //declaration as condition: test if lvalue is non-zero
        {
            return lvalue.value().value();
        }
        else
            return relational(code);
    }
    bool empty_expr(code_fragment& code)
    {
        code_fragment peek(code);
        char current = skipping_space_get_current_and_eat(peek);
        return !begin_legal_char(current);
    }
    optional<var_id> lvalue_assignment(code_fragment& code)
    {
        code_fragment assignment_fragment(code);
        auto lvalue = lvalue_var(assignment_fragment);//can be safely discarded.
        if (lvalue)
        {
            if (skipping_space_get_current_and_eat(assignment_fragment) == '=')
            {
                if (assignment_fragment.current() != '=') //if it is not equality operator
                {
                    code = assignment_fragment;
                    int rvalue = rvalue_assignment(code);
                    lvalue.value().set(rvalue);
                    return lvalue;
                }
            }
        }
        return nullopt;
    }
    int relational(code_fragment& code)
    {
        int left = term(code);
        return right_relational(left,code);
    }
    int right_relational(int left_value,code_fragment&code)
    {
        code_fragment relational_fragment(code);
        auto opt = relational_op_type(relational_fragment);
        if (opt)
        {
            relational_op op_type = opt.value();
            code = relational_fragment;
            int right = term(code);
            switch (op_type)
            {
            case relational_op::equal:
                return left_value == right;
            case relational_op::not_equal:
                return left_value != right;
            case relational_op::less_or_equal:
                return left_value <= right;
            case relational_op::less:
                return left_value < right;
            case relational_op::greater_or_equal:
                return left_value >= right;
            case relational_op::greater:
                return left_value > right;
            default:
                return 0;
            }
        }
        else
            return left_value;
    }
    optional<relational_op> relational_op_type(code_fragment& code)
    {
        char c = skipping_space_get_current_and_eat(code);
        if (c == '=')
        {
            if (code.current_and_eat() == '=')
                return relational_op::equal;
            else
                throw std::invalid_argument(std::string("Interpret failed!Expect '==' operator.In ") + __func__);
        }
        else if (c == '!')
        {
            if (code.current_and_eat() == '=')
                return relational_op::not_equal;
            else
                throw std::invalid_argument(std::string("Interpret failed!Expect '!=' operator.In ") + __func__);
        }
        else if (c == '<')
        {
            if (code.current() == '=')
            {
                code.eat();
                return relational_op::less_or_equal;
            }
            else
                return relational_op::less;
        }
        else if (c == '>')
        {
            if (code.current() == '=')
            {
                code.eat();
                return relational_op::greater_or_equal;
            }
            else
                return relational_op::greater;
        }
        return nullopt;
    }
    int term(code_fragment& code)
    {
        int left = factor(code);
        return right_term(left, code);
    }
    int right_term(int left_value, code_fragment& code)
    {
        code_fragment term_fragment(code);
        char c = skipping_space_get_current_and_eat(term_fragment);
        if (c == '+')
        {
            code = term_fragment;
            return right_term(left_value + factor(code), code);
        }
        else if (c == '-')
        {
            code = term_fragment;
            return right_term(left_value - factor(code), code);
        }
        else
            return left_value;
    }
    int factor(code_fragment& code)
    {
        int left = rvalue_prefix(code);
        return right_factor(left, code);
    }
    int right_factor(int left_value, code_fragment& code)
    {
        code_fragment factor_fragment(code);
        char op = skipping_space_get_current_and_eat(factor_fragment);
        if (op == '*')
        {
            code = factor_fragment;
            return right_factor(left_value * rvalue_prefix(code), code);
        }
        else if (op == '/')
        {
            code = factor_fragment;
            return right_factor(left_value / rvalue_prefix(code), code);
        }
        else
            return left_value;
    }
    int rvalue_prefix(code_fragment& code)
    {
        auto lvalue = lvalue_prefix(code);
        if (lvalue)
            return lvalue.value().value();
        else
            return postfix(code);
    }
    optional<var_id> lvalue_prefix(code_fragment& code)
    {
        code_fragment prefix_fragment(code);
        char op = skipping_space_get_current_and_eat(prefix_fragment);
        if (op == '+')
        {
            if (prefix_fragment.current_and_eat() == '+')
            {
                code = prefix_fragment;
                auto lvalue = lvalue_var(code);
                if (lvalue)
                {
                    var_id id = lvalue.value();
                    id.set(id.value() + 1);
                    return id;
                }
                else
                    throw std::invalid_argument(std::string("Interpret failed!'++' operator expects lvalue.In ") + __func__);
            }
            else
                throw std::invalid_argument(std::string("Interpret failed!Expect '++' operator.In ") + __func__);
        }
        else if (op == '-')
        {
            if (prefix_fragment.current_and_eat() == '-')
            {
                code = prefix_fragment;
                auto lvalue = lvalue_var(code);
                if (lvalue)
                {
                    var_id id = lvalue.value();
                    id.set(id.value() - 1);
                    return id;
                }
                else
                    throw std::invalid_argument(std::string("Interpret failed!'--' operator expects lvalue.In ") + __func__);
            }
            else
                throw std::invalid_argument(std::string("Interpret failed!Expect '--' operator.In ") + __func__);
        }
        return nullopt;
    }
    int postfix(code_fragment& code)
    {
        code_fragment postfix_fragment(code);
        auto lvalue = lvalue_var(postfix_fragment);  //TODO:也许改成调用'lvalue_primary'会更加一致?但目前似乎没有必要.
        if (lvalue)
        {
            char op = skipping_space_get_current_and_eat(postfix_fragment);
            if (op == '+')
            {
                if (postfix_fragment.current_and_eat() == '+')
                {
                    code = postfix_fragment;
                    var_id id = lvalue.value();
                    int value = id.value();
                    id.set(id.value() + 1);
                    return value;
                }
            }
            else if (op == '-')
            {
                if (postfix_fragment.current_and_eat() == '-')
                {
                    code = postfix_fragment;
                    var_id id = lvalue.value();
                    int value = id.value();
                    id.set(id.value() - 1);
                    return value;
                }
            }
        }
        return rvalue_primary(code);
    }
    int rvalue_primary(code_fragment& code)
    {
        auto var = rvalue_var(code);
        if(var)
            return var.value();
        else
        {
            auto lit = literal(code);
            if(lit)
                return lit.value();
        }
        throw std::invalid_argument(std::string("Interpret failed!Expect primary expression.In ") + __func__);
    }
    optional<int> rvalue_var(code_fragment& code)
    {
        auto var = lvalue_var(code);
        if(var)
            return var.value().value();
        else
            return nullopt;
    }
    optional<var_id> lvalue_var(code_fragment& code)
    {
        code_fragment variable_fragment(code);
        auto name = read_identifier(variable_fragment);
        if(name)
        {
            code = variable_fragment;
            if(table_.exists(name.value()))
            {
                return table_.get(name.value());
            }
            else
                throw std::invalid_argument(std::string("Interpret failed!Identifier ") + name.value() +" is undeclared.In " + __func__);
        }
        else
            return nullopt;
    }
    optional<int> literal(code_fragment& code)
    {
         auto lit = read_literal(code);
        if(lit)
            return lit.value();
        else
            return nullopt;
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
        return code.current_and_eat();
    }
    bool begin_id_char(char c)
    {
        return std::isalpha(c) || c == '_';
    }
    bool is_id_char(char c)
    {
        return std::isalnum(c) || c =='_';
    }
    optional<std::string> read_identifier(code_fragment& code)
    {
        code_fragment id_fragment(code);
        char c = skipping_space_get_current_and_eat(id_fragment);
        if (begin_id_char(c))
        {
            std::string name;
            name.push_back(c);
            while (c = id_fragment.current(), is_id_char(c))
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
        code_fragment literal_fragment(code);
        char c = skipping_space_get_current_and_eat(literal_fragment);
        if (std::isdigit(c))
        {
            std::string digits;
            digits.push_back(c);
            while (c = literal_fragment.current(), std::isdigit(c))
            {
                literal_fragment.eat();
                digits.push_back(c);
            }
            code = literal_fragment;
            return std::stoi(digits);
        }
        return nullopt;
    }
    bool begin_op_char(char c)
    {
        return c == '+' || c == '-';
    }
    bool begin_legal_char(char c)
    {
        return is_space(c) || isdigit(c) || begin_id_char(c) || begin_op_char(c);
    }
    symbol_table& table_;
    std::string const& code_;
};


#endif //SIMPLE_C_INTERPRETER_EXPRESSION_INTERPRETER_H
