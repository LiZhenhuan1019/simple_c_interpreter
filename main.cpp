#include <iostream>
#include <fstream>

#ifndef NDEBUG
#include "test/test.h"
#else
#include "symbol_table.h"
#include "expr_calc.h"
#include "preprocessor.h"
#include "codesim.hxx"
void output(std::vector<int> ints)
{
    std::ofstream output("./output.txt");
    std::size_t i = 0;
    for(;i<ints.size()-1;++i)
        output<<ints[i]<<" ";
    output<<ints[i];
}
#endif
int main()
{
#ifndef NDEBUG
    test_all();
#else

    std::ifstream input("./input.txt");
    std::string line;
    std::string code;
    while(input.good())
    {
        std::getline(input,line);
        line.push_back('\n');
        code += line;
    }
    std::cout <<code;
    std::string preprocess_code = preprocessor(code).processed_code();
    Simulator sim;
    sim.bind(preprocess_code);
    std::vector<int> lines;
    sim.runSimulation(lines);
    output(lines);
#endif
    return 0;
}