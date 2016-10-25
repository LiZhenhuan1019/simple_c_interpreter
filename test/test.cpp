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
void test_expr_calc_comma()
{
    symbol_table s;
    expr_calc expr(s);
    std::string str("12,31+12*35,5+21;");
    code_fragment f(str);
    //std::cout<<expr.value_of(f);
    assert(expr.value_of(f) == 26);
}
void test_expr_calc_assignment()
{
    symbol_table s;
    s.add("a");

    expr_calc expr(s);
    std::string str("a=5,a+5;");
    code_fragment f(str);
    //std::cout<<expr.value_of(f);
    assert(expr.value_of(f) == 10);
}
void test_expr_calc_arithmetic()
{
    symbol_table s;
    expr_calc expr(s);
    std::string str("12+31+12*35/5+21;");
    code_fragment f(str);
    //std::cout<<expr.value_of(f);
    assert(expr.value_of(f) == 148);
}
void test_expr_calc_association()
{
    symbol_table s;
    expr_calc expr(s);
    std::string str("60/3/5;");
    code_fragment f(str);
    //std::cout<<expr.value_of(f);
    assert(expr.value_of(f) == 4);
}
void test_expr_calc_relational()
{
    symbol_table s;
    expr_calc expr(s);
    std::string str("12 +13> 21;");
    code_fragment f(str);
    //std::cout<<expr.value_of(f);
    assert(expr.value_of(f) == 1);
}
void test_expr_calc_variable()
{
    symbol_table s;
    s.add("a");
    s.set("a", 5);

    expr_calc expr(s);
    std::string str("12 +13 - a +3* 21;");
    code_fragment f(str);
    //std::cout<<expr.value_of(f);
    assert(expr.value_of(f) == 83);
    str = "++a +12;";
    f = str;
    //std::cout<<expr.value_of(f);
    assert(expr.value_of(f) == 18);

    str = "5 +a++ + 23;";
    f = str;
    assert(expr.value_of(f) == 34);
}
void test_expr_calc()
{
    std::ifstream test_cases("test/test_case/expr_calc.test_case");
    std::string expression;
    symbol_table table;
    table.add("a");
    while (test_cases.good())
    {
        while (test_cases.good() && test_cases.peek() == '#')
            std::getline(test_cases, expression);//跳过以#开头的行
        int expected = -1;
        test_cases >> expected;
        std::getline(test_cases, expression);
        if (test_cases.good())
        {
            //std::cout << "expected:" << expected << " expression:" << expression << std::endl;
            expr_calc calculator(table);
            code_fragment f(expression);
            int result = calculator.value_of(f);
            if (result != expected)
                std::cout << std::string("计算错误!\n表达式:") + expression + "\n预期结果:" +
                             std::to_string(expected) + "\n实际结果:" + std::to_string(result);
        }
    }
    test_expr_calc_comma();
    test_expr_calc_assignment();
    test_expr_calc_arithmetic();
    test_expr_calc_association();
    test_expr_calc_relational();
    test_expr_calc_variable();
}
void test_all()
{
    test_symbol_table();
    test_expr_calc();
    std::cout << "Succes!" << std::endl;
}
