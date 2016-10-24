//
// Created by lizhe on 2016/10/24.
//

#include "symbol_table.h"

void symbol_table::add(std::string const& name)
{
    auto iter = int_variables.find(name);
    if (iter == int_variables.end())
        int_variables.insert(std::make_pair(name, std::vector<int>(1)));
    else
        (*iter).second.push_back(0);
}
void symbol_table::set(std::string const& name, int value)
{
    auto iter = int_variables.find(name);
    if (iter == int_variables.end())
        int_variables.insert(std::make_pair(name, std::vector<int>{value}));
    else
        (*iter).second.back() = value;
}
void symbol_table::remove(std::string const& name)
{
    auto iter = int_variables.find(name);
    if (iter != int_variables.end())
    {
        if ((*iter).second.size() > 1)
            (*iter).second.pop_back();
        else
            int_variables.erase(iter);
    }
}
int symbol_table::value(std::string const& name)
{
    auto iter = int_variables.find(name);
    if (iter != int_variables.end())
        return (*iter).second.back();
    else
        return -1; //TODO: 也许采取别的方式?
}

