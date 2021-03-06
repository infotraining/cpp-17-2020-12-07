#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

template <typename T>
struct Holder
{
    T value;

    Holder(const T& v)
        : value(v)
    {
    }

    template <typename Args>
    Holder(initializer_list<Args> lst)
        : value(lst)
    {
    }
};

template <typename Args>
Holder(initializer_list<Args>)->Holder<vector<Args>>;

template <typename T>
Holder(T)->Holder<T>;

Holder(const char*)->Holder<string>;

///////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Guess deduced argument types")
{
    int x = 42;
    int& ref_x = x;
    const int& cref_x = x;

    using TODO = void;

    Holder h1(x);
    static_assert(is_same_v<decltype(h1), Holder<int>>);

    Holder h2(ref_x);
    static_assert(is_same_v<decltype(h2), Holder<int>>);

    Holder h3(cref_x);
    static_assert(is_same_v<decltype(h3),Holder<int>>);

    int tab[10];
    Holder h4 = tab;
    static_assert(is_same_v<decltype(h4), Holder<int*>>);

    Holder h5("test");
    static_assert(is_same_v<decltype(h5), Holder<string>>);

    Holder h6{1, 2, 3, 4, 5, 6};    
    static_assert(is_same_v<decltype(h6), Holder<vector<int>>>);

    Holder h7(h6);
    static_assert(is_same_v<decltype(h7), Holder<vector<int>>>);

    Holder h8{h6, h7};
    static_assert(is_same_v<decltype(h8), Holder<vector<Holder<vector<int>>>>>);
}