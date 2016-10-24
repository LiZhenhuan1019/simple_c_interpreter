//
// Created by lizhe on 2016/10/24.
//
#include <cassert>
#include "../symbol_table.h"
void test_symbol_table()
{
    symbol_table s;
    s.add("a");
    s.set("a",5);
    assert(s.value("a") == 5);

    s.add("a");
    s.set("a",4);
    assert(s.value("a") == 4);

    s.remove("a");
    assert(s.value("a") == 5);
}
