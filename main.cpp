#include <iostream>
#include "test/test.h"
void test_all()
{

    test_symbol_table();
    std::cout << "Succes!" << std::endl;
}
int main()
{
    test_all();
    return 0;
}