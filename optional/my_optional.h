//
// Created by lizhe on 2016/10/25.
//

#ifndef OPTIONAL_MY_OPTIONAL_H
#define OPTIONAL_MY_OPTIONAL_H
#include <memory>
class nullopt_t{};
constexpr nullopt_t nullopt;
template <class ValueT>
class optional
{
public:
    using value_t = ValueT;
    optional(value_t const&v)
        :ptr(new value_t(std::move(v)))
    {}
    optional(value_t && v)
        :ptr(new value_t(std::move(v)))
    {}
    optional(nullopt_t)
        :ptr(nullptr)
    {}
    value_t &value()
    {
        return *ptr;
    }
    value_t const&value() const
    {
        return *ptr;
    }
    operator bool() const
    {
        return bool(ptr);
    }
private:
    std::unique_ptr<value_t> ptr;
};
#endif //OPTIONAL_MY_OPTIONAL_H
