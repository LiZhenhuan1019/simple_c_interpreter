//
// Created by lizhe on 2016/10/24.
//

#ifndef SIMPLE_C_INTERPRETER_SYMBOL_TABLE_H
#define SIMPLE_C_INTERPRETER_SYMBOL_TABLE_H
#include <map>
#include <vector>
#include <string>
class var_id
{
    friend class symbol_table;
public:
    using iterator_t = std::map<std::string,std::vector<int>>::iterator;
private:
    var_id(iterator_t iter)
        : iterator(iter)
    {}
public:
    var_id(var_id const&) = default;
    std::string const& name() const
    {
        return (*iterator).first;
    }
    void set(int value)
    {
        (*iterator).second.back() = value;
    }
    int value() const
    {
        return (*iterator).second.back();
    }
private:
    iterator_t iterator;
};
class symbol_table
{
public:
    var_id add(std::string const& name)
    {
        auto iter = int_variables.find(name);
        if (iter == int_variables.end())
            iter = (int_variables.insert(std::make_pair(name, std::vector<int>(1)))).first;
        else
            (*iter).second.push_back(0);
        return iter;
    }
    bool exists(std::string const& name) const
    {
        return int_variables.find(name) != int_variables.end();
    }
    var_id get(std::string const& name)
    {
        return int_variables.find(name);
    }
    void set(std::string const&name,int value)
    {
        auto iter = int_variables.find(name);
        if (iter == int_variables.end())
            int_variables.insert(std::make_pair(name, std::vector<int>{value}));
        else
            (*iter).second.back() = value;
    }
    void remove(std::string const&name)
    {
        remove(get(name));
    }
    void remove(var_id id)
    {
        auto iter = id.iterator;
        if (iter != int_variables.end())
        {
            if ((*iter).second.size() > 1)
                (*iter).second.pop_back();
            else
                int_variables.erase(iter);
        }
    }
private:
    std::map<std::string,std::vector<int>> int_variables;
};


#endif //SIMPLE_C_INTERPRETER_SYMBOL_TABLE_H
