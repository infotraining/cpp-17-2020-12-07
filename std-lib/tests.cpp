#include <algorithm>
#include <any>
#include <array>
#include <charconv>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <variant>

#include "catch.hpp"

using namespace std;

enum class Coffee : uint8_t
{
    espresso,
    chemex,
    v60
};

TEST_CASE("enums + {}")
{
    Coffee c1 = Coffee::espresso;
    // Coffee c2 = 1; // ERROR
    Coffee c3 {1};
}

namespace Explain
{
    enum class byte : uint8_t
    {
    };

    template <class IntegerType>
    constexpr byte& operator<<=(byte& b, IntegerType shift) noexcept
    {
        return b <<= shift;
    }
}

TEST_CASE("bytes in C++")
{
    uint8_t byte1 = 42;

    byte1 += 8;

    auto byte2 = (byte1 << 2);

    std::byte b1 {42};
    b1 <<= 2;

    std::byte b2 {0b00110011};
    std::byte b3 {0b11010010};

    std::byte result = b1 ^ (b2 & b3);
    result |= (b2 << 2);

    std::cout << "result: " << std::to_integer<int>(result) << std::endl;
}

////////////////////////////////////////////////////////
// string_view

TEST_CASE("string_view")
{
    SECTION("creation")
    {
        const char* c_str = "text";
        std::string str = "text";

        std::string_view sv1 = c_str;
        REQUIRE(sv1.size() == 4);

        std::string_view sv2 = str;
        REQUIRE(sv1 == sv2);

        constexpr std::string_view sv3 = "text"sv;

        std::string text = "abc def ghi";

        std::string_view token1(text.data(), 3);
        std::string_view token2(text.data() + 4, 3);
        std::cout << token1 << "\n";
        std::cout << token2 << "\n";
    }
}

template <typename Container>
void print_all(const Container& container, std::string_view prefix)
{
    cout << prefix << ": [ ";
    for (const auto& item : container)
        cout << item << " ";
    cout << "]\n";
}

TEST_CASE("using string_view")
{
    vector vec = {1, 2, 3};
    print_all(vec, "vec");
}

TEST_CASE("string_view - difference between std::string & c-string")
{
    std::string_view sv;
    REQUIRE(sv.data() == nullptr);

    std::string str;
    REQUIRE(str.data() != nullptr);

    array text_array = {'t', 'e', 'x', 't'};
    std::string_view sv_text(text_array.data(), text_array.size());

    std::cout << sv_text << "\n";

    // std::ifstream fin(sv_text.data()); // UB - sv_text is not null-terminated string
}

TEST_CASE("string_view conversions")
{
    std::string text = "abc";
    std::string_view sv_text = text; // implicit conversion

    std::string str_text = std::string(sv_text); // explicit conversion
}

std::string_view get_prefix(std::string_view text, size_t length)
{
    return {text.data(), length};
}

TEST_CASE("beware - dangling string_view")
{
    std::string_view p1 = get_prefix("abc.def", 3);
    REQUIRE(p1 == "abc"sv);

    // std::string_view p2 = get_prefix("abc.def"s, 3);  // dangling string_view
    // REQUIRE(p2 == "abc"sv);

    //std::string_view dangling = "abc"s;
}

/////////////////////////////////////////////////////
// std::optional

TEST_CASE("optional")
{
    std::optional<int> o1 = 42;
    REQUIRE(o1.has_value());

    std::optional<int> o2 {42};
    REQUIRE(o2.has_value());

    std::optional<int> o3;
    REQUIRE(o3.has_value() == false);

    std::optional<int> o4 = std::nullopt;
    REQUIRE(o4.has_value() == false);

    std::optional o5 = "text"s; // std::optional<std::string> - CTAD

    std::optional<std::atomic<int>> opt_atomic(std::in_place, 42);
    REQUIRE(opt_atomic.has_value());

    if (opt_atomic)
    {
        std::cout << *opt_atomic << "\n";
    }

    SECTION("access to value")
    {
        optional<int> number = 42;

        SECTION("safe")
        {
            if (number)
            {
                REQUIRE(*number == 42);
            }
        }

        SECTION("value()")
        {
            REQUIRE(number.value() == 42);

            number.reset();
            number = std::nullopt;

            REQUIRE_THROWS_AS(number.value(), std::bad_optional_access);
        }
    }
}

std::optional<const char*> maybe_getenv(const char* n)
{
    if (const char* x = std::getenv(n))
        return x;
    else
        return std::nullopt;
}

TEST_CASE("value_or()")
{
    auto result = maybe_getenv("wrong PATH").value_or("not found");

    std::cout << result << "\n";
}

TEST_CASE("optional & move semantics")
{
    optional<string> text = "text";

    optional target = std::move(text);

    REQUIRE(target.has_value());
    REQUIRE(target.value() == "text"s);

    REQUIRE(text.has_value()); // text still has value
    text.reset();
}

TEST_CASE("optional - strange cases")
{
    SECTION("optional ==")
    {
        optional o1 = 42;

        REQUIRE(o1 == 42);

        o1.reset();

        REQUIRE_FALSE(o1 == 42);
    }

    SECTION("optional<bool>")
    {
        optional<bool> ob {false};

        if (ob)
        {
            std::cout << "ob has value..." << *ob << "\n";
        }

        REQUIRE(ob == false);
    }

    SECTION("optional<int*>")
    {
        std::optional<int*> optr {nullptr};

        if (optr)
        {
            std::cout << "optr has value..." << *optr << "\n";
        }

        REQUIRE(optr == nullptr);
    }
}

std::optional<int> to_int(string_view str)
{
    int result {};

    auto start = str.data();
    auto end = str.data() + str.size();

    if (const auto [pos_end, error_code] = std::from_chars(start, end, result);
        error_code != std::errc {} || pos_end != end)
    {
        return std::nullopt;
    }

    return result;
}

TEST_CASE("to_int")
{
    optional<int> number = to_int("42");
    REQUIRE(number.has_value());
    REQUIRE(number == 42);

    optional<int> wrong_number = to_int("4h2");
    REQUIRE(wrong_number.has_value() == false);
}

////////////////////////////////////////////////////////
// std::any

TEST_CASE("std::any")
{
    std::any anything;

    REQUIRE(anything.has_value() == false);

    anything = 1;
    anything = 3.14;
    anything = std::string("text");
    anything = vector {1, 2, 3};

    REQUIRE(anything.has_value());

    SECTION("std::any & RTTI")
    {
        REQUIRE(anything.type() == typeid(std::vector<int>));
        std::cout << "anything - type: " << anything.type().name() << "\n";
    }

    SECTION("std::any_cast")
    {
        auto vec = std::any_cast<std::vector<int>>(anything);
        REQUIRE(vec == vector {1, 2, 3});

        REQUIRE_THROWS_AS(std::any_cast<std::string>(anything), std::bad_any_cast);
    }

    SECTION("std::any_cast with pointer")
    {
        vector<int>* ptr_vec = std::any_cast<std::vector<int>>(&anything);

        if (ptr_vec)
        {
            REQUIRE(*ptr_vec == vector {1, 2, 3});
        }

        REQUIRE(std::any_cast<std::string>(&anything) == nullptr);
    }
}

class KeyValueDictionary
{
    std::map<std::string, std::any> dict_;

public:
    template <typename T>
    optional<map<string, any>::iterator> insert(string key, T value)
    {
        auto [pos, was_inserted] = dict_.emplace(move(key), move(value));

        if (was_inserted)
            return pos;
        
        return nullopt;
    }

    template <typename T>
    T& at(const string& key)
    {
        T* value = any_cast<T>(&dict_.at(key));

        if (!value)
            throw bad_any_cast();

        return *value;
    }
};

TEST_CASE("KeyValueDictionary")
{
    KeyValueDictionary dict;

    dict.insert("name", "Jan"s);
    dict.insert("age", 33);
    dict.insert("data", vector{1, 2, 3});

    REQUIRE(dict.at<std::string>("name") == "Jan"s);
    REQUIRE(dict.at<int>("age") == 33);
}

//////////////////////////////////////////////////////////////////
// std::variant

struct NoDefaultConstructible
{
    int value;

    NoDefaultConstructible(int v)
        : value {v}
    {
    }
};

TEST_CASE("std::variant")
{
    std::variant<int, double, std::string> v1;
    REQUIRE(v1.index() == 0);
    REQUIRE(std::holds_alternative<int>(v1) == true);

    v1 = 3.14;
    v1 = "text"s;

    REQUIRE(std::holds_alternative<std::string>(v1) == true);
    REQUIRE(v1.index() == 2);

    REQUIRE(std::get<std::string>(v1) == "text"s);
    REQUIRE_THROWS_AS(std::get<int>(v1), std::bad_variant_access);

    std::string* ptr_str = std::get_if<std::string>(&v1);

    if (ptr_str)
    {
        REQUIRE(*ptr_str == "text"s);
    }

    SECTION("monostate")
    {
        std::variant<std::monostate, NoDefaultConstructible, int, double> v1;
        REQUIRE(v1.index() == 0);
    }
}

struct Printer
{
    void operator()(int x) { std::cout << "int: " << x << "\n"; }
    void operator()(double d) { std::cout << "double: " << d << "\n"; }
    void operator()(const std::string& str) { std::cout << "string: " << str << "\n"; }
    void operator()(const std::vector<int>& v) { std::cout << "vector: " << v.size() << "\n"; }
};

TEST_CASE("visiting variants")
{
    std::variant<int, double, std::string, std::vector<int>> v = vector{1, 2, 3};

    std::visit(Printer{}, v);
}