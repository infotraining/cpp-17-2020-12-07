#include <algorithm>
#include <iostream>
#include <map>
#include <mutex>
#include <numeric>
#include <queue>
#include <string>
#include <vector>
#include <string_view>

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

//////////////////////////////////////////////////
// if constexpr + variadic templates

namespace BeforeCpp17
{
    void print()
    {
        std::cout << "\n";
    }

    template <typename Head, typename... Tail>
    void print(const Head& head, const Tail&... tail)
    {
        std::cout << head << " ";
        print(tail...);
    }
}

namespace SinceCpp17
{
    template <typename Head, typename... Tail>
    void print(const Head& head, const Tail&... tail)
    {
        std::cout << head << " ";
        
        if constexpr(sizeof...(tail) > 0)
        {
            print(tail...);
        }
        else
        {
            std::cout << "\n";
        }        
    }
}

TEST_CASE("if constexpr + variadic templates")
{
    using namespace SinceCpp17;

    print(1, 3.14, "text"s);
    // print(head = 1, tail = (3.14, "text"s)...);
    //    -> print(head = 3.14, tail = ("text"s)...)
    //        -> print(head = "text"s, tail = ()...)
    //            -> print()

    print(1, 3.14, "text"s, "abc"sv);
}

template <typename TSource, typename TTarget, size_t NSource, size_t NTarget>
void my_copy(TSource(&source)[NSource], TTarget(&target)[NTarget])
{
    static_assert(NTarget >= NSource, "size of target array must be larger than size of source array");

    // T1 == T2 && T1 is trivially copyable
    if constexpr(std::is_same_v<TSource, TTarget> && std::is_trivially_copyable_v<TSource>)
    {
        memcpy(target, source, NSource * sizeof(TSource));
        std::cout << "copy by memcpy...\n";
    }
    else
    {
        for(size_t i = 0; i < NSource; ++i)
            target[i] = source[i];
        std::cout << "copy using loop...\n";
    }
}


TEST_CASE("mcopy")
{
    SECTION("copy using loop")
    {
        std::string words1[] = {"one", "two", "three"};
        std::string_view words2[3] = {};

        my_copy(words1, words2);

        REQUIRE(std::equal(begin(words2), end(words2), begin(words1)));
    }

    SECTION("int")
    {
        int tab1[] = {1, 2, 3};
        int tab2[3];

        my_copy(tab1, tab2);

        REQUIRE(std::equal(begin(tab1), end(tab1), begin(tab2)));
    }
}