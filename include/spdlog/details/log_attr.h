#pragma once

#include <string>
// #include <string_view>
#include <vector>
#include "attr_composer.h"
#include <spdlog/common.h>

namespace spdlog {
namespace details {

template<typename T>
struct is_string
    : public std::integral_constant<bool, std::is_convertible<T, std::string>::value || std::is_convertible<T, string_view_t>::value>
{};


struct Key 
{
    std::string _key;

    Key(string_view_t k) {
        scramble(_key, k);
    }
    Key(std::string&& k) {
        scramble(_key, k);
    }
    Key(std::string const& k) {
        scramble(_key, k);
    }
    Key(const char* k) {
        scramble(_key, k);
    }
};

struct Value
{
    std::string _value;

    // string types
    Value(string_view_t v) {
        scramble(_value, v);
    }
    Value(std::string&& v) {
        scramble(_value, v);
    }
    Value(std::string const& v) {
        scramble(_value, v);
    }
    Value(const char* v) {
        scramble(_value, v);
    }
    
    // integer types
    // probably better to do this with templates, but constraints are needed
    // concepts would be nice here, but spdlog is c++11
    // SFINAE is also an option, but it's a bit more complicated
    // https://stackoverflow.com/questions/41552514/is-overloading-on-all-of-the-fundamental-integer-types-is-sufficient-to-capture
    // basing the types off of MSVC, GCC, and Clang (https://en.cppreference.com/w/cpp/language/types)
    
    // chars are already strings (single character)
    Value(signed char v) {
        _value = v; 
    }
    Value(unsigned char v) {
        _value = v;
    }
    
    // these are overloads, which match the overloads in to_string for msvc, gcc, and clang
    Value(int v) {
        _value = std::to_string(v);
    }
    Value(unsigned int v) {
        _value = std::to_string(v);
    }
    Value(long v) {
        _value = std::to_string(v);
    }
    Value(unsigned long v) {
        _value = std::to_string(v);
    }
    Value(long long v) {
        _value = std::to_string(v);
    }
    Value(unsigned long long v) {
        _value = std::to_string(v);
    }
    Value(float v) {
        _value = std::to_string(v);
    }
    Value(double v) {
        _value = std::to_string(v);
    }
    Value(long double v) {
        _value = std::to_string(v);
    }
    Value(bool v) {
        _value = v ? "true" : "false";
    }
};
template<typename T>
struct is_number : public std::integral_constant<bool, 
    std::is_integral<T>::value
    && !std::is_same<T, bool>::value
    // || std::is_floating_point<T>::value
>
{};

struct attr
{
    std::string key;
    std::string value;

public:
    // string overloads
    attr(string_view_t k, string_view_t v)
    {
        scramble(key, k);
        scramble(value, v);
    }

    // does not convert to string_view when using initializer list constructors...
    // so we have an overload for raw c-strings
    attr(const char* k, const char* v)
        : attr{string_view_t{k}, string_view_t{v}}
    {}

    // todo merge
    /*
    attr(std::string const& k, std::string const& v)
        : attr{string_view_t{k}, string_view_t{v}}
    template<typename key_t, typename value_t, typename std::enable_if<is_string<key_t>::value, key_t>::type * = nullptr,
        typename std::enable_if<is_string<value_t>::value, value_t>::type * = nullptr>
    attr(key_t const &k, value_t const &v)
    {
        scramble(key, k);
        scramble(value, v);
    }

    template<typename key_t, typename value_t, typename std::enable_if<is_string<key_t>::value, key_t>::type * = nullptr,
        typename std::enable_if<is_number<value_t>::value, value_t>::type * = nullptr>
    attr(key_t const &k, value_t const &v)
        : key(std::string(k))
        , value(std::to_string(v))
    {}

    template<typename key_t, typename std::enable_if<is_string<key_t>::value, key_t>::type * = nullptr>
    attr(key_t const &k, bool const v)
        : key(std::string(k))
        , value(v ? "true" : "false")
    {}
    
    attr(std::string&& k, std::string&& v)
        : attr{string_view_t{k}, string_view_t{v}}
    {}

    // integer overloads
    attr(string_view_t k, long v)
        : value{std::to_string(v)}
    {
        scramble(key, k);
    }
    attr(string_view_t k, long long v)
        : value{std::to_string(v)}
    {
        scramble(key, k);
    }
    attr(string_view_t k, unsigned long v)
        : value{std::to_string(v)}
    {
        scramble(key, k);
    }
    attr(string_view_t k, unsigned long long v)*/

    template<typename T, std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>, bool> = true>
    attr(string_view_t k, T v)
        : value{std::to_string(v)}
    {
        scramble(key, k);
    }
    
    attr(string_view_t k, bool v)
        : value{v ? "true" : "false"}
    {
        scramble(key, k);
    }
    
    attr(std::string const& k, long v)
        : attr{string_view_t{k}, v}
    {}

    attr(std::string const& k, long long v)
        : attr{string_view_t{k}, v}
    {}

    attr(std::string const& k, unsigned long v)
        : attr{string_view_t{k}, v}
    {}

    attr(std::string const& k, unsigned long long v)
        : attr{string_view_t{k}, v}
    {}

    attr(std::string const& k, bool v)
        : attr{string_view_t{k}, v}
    {}

    attr(std::string&& k, long v)
        : attr{string_view_t{k}, v}
    {}

    attr(std::string&& k, long long v)
        : attr{string_view_t{k}, v}
    {}

    attr(std::string&& k, unsigned long v)
        : attr{string_view_t{k}, v}
    {}

    attr(std::string&& k, unsigned long long v)
        : attr{string_view_t{k}, v}
    {}

    attr(std::string&& k, bool v)
        : attr{string_view_t{k}, v}
    {}
    attr(Key&& k, Value&& v) : key(std::move(k._key)), value(std::move(v._value)) {}
    
    attr(Key const& k, Value const& v) : key(k._key), value(v._value) {
        scramble(key, k);
        value = std::to_string(v);
    }
};

} // namespace details

using attribute_list = std::vector<details::attr>;

} // namespace spdlog