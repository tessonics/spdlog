#pragma once

#include <string>
#include <vector>
#include "attr_composer.h"
#include <spdlog/common.h>

namespace spdlog {
namespace details {

// template<typename T>
// concept composable = std::same_as<T, bool> || std::integral<T> || std::floating_point<T> || std::convertible_to<T, std::string_view>;


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
    // https://stackoverflow.com/questions/41552514/is-overloading-on-all-of-the-fundamental-integer-types-is-sufficient-to-capture
    
    // int8_t
    Value(signed char v) {
        _value = std::to_string(v);
    }
    // uint8_t
    Value(unsigned char v) {
        _value = std::to_string(v);
    }
    // int16_t
    Value(short v) {
        _value = std::to_string(v);
    }
    // uint16_t
    Value(unsigned short v) {
        _value = std::to_string(v);
    }
    // int32_t
    Value(int v) {
        _value = std::to_string(v);
    }
    // uint32_t
    Value(unsigned int v) {
        _value = std::to_string(v);
    }
    // int64_t
    Value(long v) {
        _value = std::to_string(v);
    }
    // uint64_t
    Value(unsigned long v) {
        _value = std::to_string(v);
    }
    // some compilers use long long for int64_t
    Value(long long v) {
        _value = std::to_string(v);
    }
    // some compilers use unsigned long long for uint64_t
    Value(unsigned long long v) {
        _value = std::to_string(v);
    }
    Value(bool v) {
        _value = v ? "true" : "false";
    }
};

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
    attr(Key const& k, Value const& v) : key(k._key), value(v._value) {}
};

} // namespace details

using attribute_list = std::vector<details::attr>;

} // namespace spdlog