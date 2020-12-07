#include <algorithm>
#include <iostream>
#include <map>
#include <mutex>
#include <numeric>
#include <queue>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

TEST_CASE("if with initializers")
{
    vector vec = {1, 42, 665, 345, 24564, 3452};

    if (auto pos = std::find(begin(vec), end(vec), 665); pos != end(vec))
    {
        std::cout << "Found... " << *pos << "\n";
    }
    else
    {
        std::cout << "Item was not found...\n";
        assert(pos == end(vec));
    }
}

TEST_CASE("if with initializers + structured bindings")
{
    std::map<int, string> dict = {{1, "jeden"}};

    if (auto [pos, was_inserted] = dict.emplace(2, "dwa"); was_inserted)
    {
        const auto& [key, value] = *pos;
        std::cout << "Item: " << key << " - " << value << " was inserted...\n";
    }
    else
    {
        const auto& [key, value] = *pos;
        std::cout << "Item: " << key << " - " << value << " was already in map...\n";
    }
}

TEST_CASE("if with mutex")
{
    std::queue<std::string> q_msg;
    std::mutex mtx_q_msg;

    // thread #1
    {
        std::lock_guard lk {mtx_q_msg};
        q_msg.push("START");
    }

    SECTION("thread #2")
    {
        if (std::lock_guard lk {mtx_q_msg}; !q_msg.empty())
        {
            auto msg = q_msg.front();
            q_msg.pop();
        } // mtx_q_msg.unlock()
    }
}

///////////////////////////////////////////////////////////////////////////
/// constexpr if

namespace BeforeCpp17
{
    template <typename T>
    auto is_power_of_2(T value) -> std::enable_if_t<std::is_integral_v<T>, bool>
    {
        return value > 0 && (value & (value - 1)) == 0;
    }

    template <typename T>
    auto is_power_of_2(T value) -> std::enable_if_t<std::is_floating_point_v<T>, bool>
    {
        int exponent;
        const T mantissa = std::frexp(value, &exponent);
        return mantissa == T(0.5);
    }
}

template <typename T>
bool is_power_of_2(T value)
{
    if constexpr(std::is_integral_v<T>)
    {
        return value > 0 && (value & (value - 1)) == 0;
    }
    else
    {
        int exponent;
        const T mantissa = std::frexp(value, &exponent);
        return mantissa == T(0.5);
    }
}

TEST_CASE("constexpr if")
{
    REQUIRE(is_power_of_2(8u));
    REQUIRE(is_power_of_2(5) == false);
    REQUIRE(is_power_of_2(64));
    REQUIRE(is_power_of_2(1024));

    REQUIRE(is_power_of_2(8.0));
    REQUIRE(is_power_of_2(8.0f));
}

//////////////////////////////////////////////////////////
// type traits

int foo(int x)
{
    return 2 * x;
}

template <int v>
struct Foo
{
    static constexpr int value = 2 * v;
};

template <typename T>
struct IsPointer
{
    static constexpr bool value = false;
};

template <typename T>
struct IsPointer<T*>
{
    static constexpr bool value = true;
};

// template  variable
template <typename T>
constexpr bool IsPointer_v = IsPointer<T>::value;

TEST_CASE("type traits")
{
    REQUIRE(foo(3) == 6);

    static_assert(Foo<3>::value == 6);

    using Type1 = int;
    static_assert(IsPointer_v<Type1> == false);

    using Type2 = int*;
    static_assert(std::is_pointer_v<Type2> == true);
}

template <typename T>
std::string to_str(T value)
{
    if constexpr(std::is_same_v<T, std::string>)
    {
        return value;
    }
    else if constexpr (std::is_arithmetic_v<T>)
    {
        return std::to_string(value);
    }
    else
    {
        return std::string(value);
    }
}

TEST_CASE("to_str")
{
    SECTION("std::string")
    {
        auto v = "str"s;
        REQUIRE(to_str(v) == "str"s);
    }

    SECTION("numbers")
    {
        REQUIRE(to_str(42) == "42"s);
    }

    SECTION("c-string")
    {
        REQUIRE(to_str("text") == "text"s);
    }
}