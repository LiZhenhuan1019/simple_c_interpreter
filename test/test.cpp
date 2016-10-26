//
// Created by lizhe on 2016/10/24.
//
#include <cassert>
#include <iostream>
#include <fstream>
#include "../symbol_table.h"
#include "../expr_calc.h"

void test_symbol_table()
{
    symbol_table s;
    s.add("a");
    s.set("a", 5);
    assert(s.get("a").value() == 5);

    s.add("a");
    s.set("a", 4);
    assert(s.get("a").value() == 4);

    s.remove("a");
    assert(s.get("a").value() == 5);
}
bool test_expr_calc()
{
    bool success = true;
    std::ifstream test_cases("test/test_case/expr_calc.test_case");
    std::string expression;
    symbol_table table;
    table.add("a");
    table.add("underscore_id");
    while (test_cases.good())
    {
        while (test_cases.good() && test_cases.peek() == '#')
            std::getline(test_cases, expression);//跳过以#开头的行
        int expected = -1;
        test_cases >> expected;
        std::getline(test_cases, expression);
        if (!test_cases.eof())
        {

            expr_calc calculator(table,expression);
            int result = calculator.value_of_expr();
            //std::cout << "expected:" << expected << " result:"<<result<<" expression:" << expression << std::endl;
            if (result != expected)
            {
                std::cout << std::string("计算错误!\n表达式:") + expression + "\n预期结果:" +
                             std::to_string(expected) + "\n实际结果:" + std::to_string(result) + "\n";
                success = false;
            }
        }
    }
    return success;
}
void test_all()
{
    bool success = true;
    test_symbol_table();
    success = success && test_expr_calc();
    if (success)
        std::cout << "Succes!" << std::endl;
    else
        std::cout << "Failed!" << std::endl;
}
