//
// Created by lizhe on 2016/10/24.
//
#include <cassert>
#include <iostream>
#include "../symbol_table.h"
#include "../expr_calc.h"
void test_symbol_table()
{
    symbol_table s;
    s.add("a");
    s.set("a",5);
    assert(s.get("a").value() == 5);

    s.add("a");
    s.set("a",4);
    assert(s.get("a").value() == 4);

    s.remove("a");
    assert(s.get("a").value() == 5);
}
void test_expr_calc0()
{
    symbol_table s;
    expr_calc expr(s);
    std::string str("12+31+12*35/5+21;");
    code_fragment f(str);
    //std::cout<<expr.value_of(f);
    assert(expr.value_of(f) == 148);
}
void test_expr_calc1()
{
    symbol_table s;
    expr_calc expr(s);
    std::string str("12 +13> 21;");
    code_fragment f(str);
    //std::cout<<expr.value_of(f);
    assert(expr.value_of(f) == 1);
}

void test_expr_calc()
{
    test_expr_calc0();
    test_expr_calc1();
}
void test_all()
{
    test_symbol_table();
    test_expr_calc();
    std::cout << "Succes!" << std::endl;
}
