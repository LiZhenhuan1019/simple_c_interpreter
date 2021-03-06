//
// Created by lizhe on 2016/10/24.
//
#include <cassert>
#include <iostream>
#include <fstream>
#include "../symbol_table.h"
#include "../expr_calc.h"
#include "../preprocessor.h"
#include "../codesim.hxx"
void test_preprocessor()
{
    std::string str= R"~(1
2/*comment1
*/3endcomment1//comment2 /*still comment
4beginliteral"string literal"endliteral
5/*comment*/end
6)~";
    preprocessor processor(str);
    std::cout<<processor.processed_code();
}
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
            std::cout << "expected:" << expected << " result:"<<result<<" expression:" << expression << std::endl;
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
void print_vector(std::vector<int> v)
{
    for(auto i: v)
        std::cout<<i<<" ";
}
void test_codesim()
{
    {
        std::cout<<"test1 begin----------------------------------\n";
        std::ifstream test1("test/test_case/test1/input.txt");
        std::string line;
        std::string code;
        while(test1.good())
        {
            std::getline(test1,line);
            line.push_back('\n');
            code += line;
        }
        std::cout <<code;
        std::string preprocess_code = preprocessor(code).processed_code();
        Simulator sim;
        sim.bind(preprocess_code);
        std::vector<int> lines;
        sim.runSimulation(lines);
        std::vector<int> expected{5, 6, 7, 9, 10, 12, 13, 15};
        assert(lines == expected);
        print_vector(lines);
        std::cout<<"\ntest1 end.--------------------------------\n";
    }
    {
        std::cout<<"test2 begin----------------------------------\n";
        std::ifstream test2("test/test_case/test2/input.txt");
        std::string line;
        std::string code;
        while(test2.good())
        {
            std::getline(test2,line);
            line.push_back('\n');
            code += line;
        }
        std::cout <<code;
        std::string preprocess_code = preprocessor(code).processed_code();
        Simulator sim;
        sim.bind(preprocess_code);
        std::vector<int> lines;
        sim.runSimulation(lines);
        std::vector<int> expected{3, 4, 5, 6, 7, 10, 14, 16};
        print_vector(lines);
        assert(lines == expected);
        std::cout<<"\ntest2 end.--------------------------------\n";
    }
    {
        std::cout<<"test3 begin----------------------------------\n";
        std::ifstream test3("test/test_case/test3/input.txt");
        std::string line;
        std::string code;
        while(test3.good())
        {
            std::getline(test3,line);
            line.push_back('\n');
            code += line;
        }
        std::cout <<code;
        std::string preprocess_code = preprocessor(code).processed_code();
        Simulator sim;
        sim.bind(preprocess_code);
        std::vector<int> lines;
        sim.runSimulation(lines);
        std::vector<int> expected{1, 2, 3, 4, 5, 6, 7, 5, 6, 7, 5, 6, 7, 5, 3, 4, 5, 3};
        assert(lines == expected);
        print_vector(lines);
        std::cout<<"\ntest3 end.--------------------------------\n";
    }
    {
        std::cout<<"test4 begin----------------------------------\n";
        std::ifstream test4("test/test_case/test4/input.txt");
        std::string line;
        std::string code;
        while(test4.good())
        {
            std::getline(test4,line);
            line.push_back('\n');
            code += line;
        }
        std::cout <<code;
        std::string preprocess_code = preprocessor(code).processed_code();
        Simulator sim;
        sim.bind(preprocess_code);
        std::vector<int> lines;
        sim.runSimulation(lines);
        std::vector<int> expected{1, 2, 3, 4, 6, 7, 9, 6, 7, 9, 6, 7, 9, 3, 4, 6, 7, 9, 3};
        assert(lines == expected);
        print_vector(lines);
        std::cout<<"\ntest4 end.--------------------------------\n";
    }
}
void test_all()
{
    bool success = true;
    //test_preprocessor();
    test_symbol_table();
    success = success & test_expr_calc();
    test_codesim();
    if (success)
        std::cout << "Succes!" << std::endl;
    else
        std::cout << "Failed!" << std::endl;
}
