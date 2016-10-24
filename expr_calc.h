//
// Created by lizhe on 2016/10/24.
//

#ifndef SIMPLE_C_INTERPRETER_EXPRESSION_INTERPRETER_H
#define SIMPLE_C_INTERPRETER_EXPRESSION_INTERPRETER_H
#include "code_fragment.h"
#include "symbol_table.h"

class symbol_table;

class expr_calc
{
public:
    expr_calc(symbol_table& table)
        : table_(table)
    {

    }
    int value_of(code_fragment& code)
    {
        

    }
private:
    var_id assignment_expr()
    {

    }
    int relational_expr(int left_value)
    {

    }
    symbol_table& table_;
};


#endif //SIMPLE_C_INTERPRETER_EXPRESSION_INTERPRETER_H
