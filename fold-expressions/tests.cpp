#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

namespace Cpp98
{
    void fold_98()
    {
        std::vector<double> vec = {1, 2.9, 3, 4, 5};

        auto sum = std::accumulate(std::begin(vec), std::end(vec), 0.0); // operator +
        std::cout << "sum: " << sum << "\n";

        auto result = std::accumulate(std::begin(vec), std::end(vec), "0"s,
            [](const std::string& reduced, int item) {
                return "("s + reduced + " + "s + std::to_string(item) + ")"s;
            });

        std::cout << result << "\n";
    }
}

namespace BeforeCpp17
{
    template <typename T>
    auto sum(const T& last)
    {
        return last;
    }

    template <typename THead, typename... TTail>
    auto sum(const THead& head, const TTail&... tail)
    {
        return head + sum(tail...);
    }
}

/////////////////////////////
// fold expression - sum (+)

template <typename... TArgs>
auto sum(const TArgs&... args)  // sum(arg1, arg2, arg3, arg4)
{
    // left unary fold
    return (... + args);  // return (((arg1 + arg2) + arg3) + arg4);
}

template <typename... TArgs>
auto sum_right(const TArgs&... args)  // sum(arg1, arg2, arg3, arg4)
{
    // right unary fold
    return (args + ...);  // return (arg1 + (arg2 + (arg3 + arg4)));
}

////////////////////////////////////
// fold expressions - print <<

template <typename... TArgs>
void print(const TArgs&... args)
{
    bool is_first = true;

    auto with_space = [&is_first](const auto& arg) {
        if (!is_first)
            std::cout << " ";
        is_first = false;
        return arg;
    };

    // left binary fold
    (std::cout << ... << with_space(args)) << "\n"; // (((std::cout << arg1) << arg2) << arg3);
}

////////////////////////////////////////
// fold expression - operator ,

template <typename... TArgs>
void print_lines(const TArgs&... args)
{
    // std::cout << arg1 << "\n";
    // std::cout << arg2 << "\n";
    // std::cout << arg2 << "\n";

    (..., (std::cout << args << "\n")); // left unary fold - operator ,
}

void f(int x)
{
    std::cout << "f(" << x << ")\n";
}

template <typename TFunc, typename... TArgs>
decltype(auto) call(TFunc func, TArgs&&... args)
{
    return (..., func(std::forward<TArgs>(args)));
}

TEST_CASE("fold")
{
    vector<int> vec;

    Cpp98::fold_98();

    REQUIRE(BeforeCpp17::sum(1, 2, 3, 4, 5) == 15);

    print(1, 3.14, "text"s);
    print_lines(1, 3.14, "text"s, "abc");

    call(f, 1, 2, 3, 4);
}

////////////////////////////////////////
// identity element - operator &&

template <typename... Args>
bool all_true(const Args&... args)
{
    return (... && args);
}

TEST_CASE("identity element")
{
    REQUIRE(all_true() == true);
    REQUIRE(all_true(true, true, false) == false);
}