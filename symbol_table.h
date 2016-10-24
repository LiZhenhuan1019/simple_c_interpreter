//
// Created by lizhe on 2016/10/24.
//

#ifndef SIMPLE_C_INTERPRETER_SYMBOL_TABLE_H
#define SIMPLE_C_INTERPRETER_SYMBOL_TABLE_H
#include <map>
#include <vector>
#include <string>
class symbol_table
{
public:
    void add(std::string const&);
    void set(std::string const&name,int value);
    void remove(std::string const&name);
    int value(std::string const&name);
private:
    std::map<std::string,std::vector<int>> int_variables;
};


#endif //SIMPLE_C_INTERPRETER_SYMBOL_TABLE_H
