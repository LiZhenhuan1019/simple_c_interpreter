//
// Created by lizhe on 2016/10/24.
//

#ifndef SIMPLE_C_INTERPRETER_SYMBOL_TABLE_H
#define SIMPLE_C_INTERPRETER_SYMBOL_TABLE_H
#include <map>
#include <string>
class symbol_table
{
public:
    void set(std::string name,int value);
    void remove(std::string name);
    int value(std::string name);
private:
    std::map<std::string,int> int_variables;
};


#endif //SIMPLE_C_INTERPRETER_SYMBOL_TABLE_H
