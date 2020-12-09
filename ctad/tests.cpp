#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <list>
#include <array>

#include "catch.hpp"

using namespace std;

void foo()
{
}

template <typename T>
void deduce1(T arg)
{
    puts(__PRETTY_FUNCTION__);
}

template <typename T>
void deduce2(T& arg)
{
    puts(__PRETTY_FUNCTION__);
}

template <typename T>
void deduce3(T&& arg)
{
    [[maybe_unused]] auto target = std::forward<T>(arg);

    puts(__PRETTY_FUNCTION__);
}

TEST_CASE("Template Argument Deduction - case 1")
{
    int x = 10;
    const int cx = 20;
    int& ref_x = x;
    const int& cref_x = cx;
    int tab[10];

    [[maybe_unused]] auto ax1 = x;  // int
    deduce1(x);

    [[maybe_unused]] auto ax2 = cx; // int
    deduce1(cx);

    [[maybe_unused]] auto ax3 = ref_x; // int
    deduce1(ref_x);

    [[maybe_unused]] auto ax4 = cref_x; // int
    deduce1(cref_x);

    [[maybe_unused]] auto ax5 = tab; // int*
    deduce1(tab); 

    [[maybe_unused]] auto ax6 = foo; // void(*)()
    deduce1(foo);
}

TEST_CASE("--")
{
    cout << "\n----------\n\n";
}

TEST_CASE("Template Argument Deduction - case 2")
{
    int x = 10;
    const int cx = 20;
    int& ref_x = x;
    const int& cref_x = cx;
    int tab[10];

    [[maybe_unused]] auto& ax1 = x; // int&
    deduce2(x);

    [[maybe_unused]] auto& ax2 = cx; // const int&
    deduce2(cx);

    [[maybe_unused]] auto& ax3 = ref_x; // int&
    deduce2(ref_x);
    deduce2<int&>(ref_x);

    [[maybe_unused]] auto& ax4 = cref_x; // const int&
    deduce2(cref_x);

    [[maybe_unused]] auto& ax5 = tab; // int(&)[10]
    deduce2(tab);

    [[maybe_unused]] auto& ax6 = foo; // void(&)()
    deduce2(foo);
}

TEST_CASE("**")
{
    cout << "\n----------\n\n";
}

TEST_CASE("Template Argument Deduction - case 3")
{
    int x = 10;

    [[maybe_unused]] auto&& a1 = x;  // int&
    deduce3(x);

    [[maybe_unused]] auto&& a2 = 10; // int&&
    deduce3(10);
}

/////////////////////////////////
// perfect forward

struct Gadget
{
    std::string name;

    Gadget(std::string n) : name{std::move(n)}
    {}
};

void have_fun(Gadget& g)
{
    puts(__PRETTY_FUNCTION__);
}

void have_fun(const Gadget& g)
{
    puts(__PRETTY_FUNCTION__);
}

void have_fun(Gadget&& g)
{
    puts(__PRETTY_FUNCTION__);
}

// void use(Gadget& g)
// {
//     have_fun(g);
// }

// void use(const Gadget& g)
// {
//     have_fun(g);
// }

// void use(Gadget&& g)
// {
//     have_fun(std::move(g));
// }

template <typename TGadget>
void use(TGadget&& g)
{
    puts(__PRETTY_FUNCTION__);
    have_fun(std::forward<TGadget>(g));
}

TEST_CASE("using gadget")
{
    Gadget g{"ipad"};
    const Gadget cg{"const ipad"};

    use(g);
    use(cg);
    use(Gadget{"temp ipad"});
}

TEST_CASE("using perfect forwarding")
{
    std::unique_ptr<Gadget> ptr{new Gadget("ipad")};
    auto ptr2 = std::make_unique<Gadget>("ipad");

    vector<Gadget> gs;
    gs.push_back(Gadget("ipad"));
    gs.emplace_back("ipad");
}

/////////////////////////////////////////////////
// CTAD

template <typename T1, typename T2>
struct ValuePair
{
    T1 fst;
    T2 snd;

    ValuePair(const T1& f, const T2& s) : fst{f}, snd{s}
    {}
};

// deduction guides
template <typename T1, typename T2>
ValuePair(T1, T2) -> ValuePair<T1, T2>;

ValuePair(const char*, const char*) -> ValuePair<std::string, std::string>;

TEST_CASE("CTAD")
{
    ValuePair<int, double> vp1(1, 3.14);

    ValuePair vp3(1, 3.14); // ValuePair<int, double>
    ValuePair vp4(3.14f, "text"s); // ValuePair<float, std::string>

    const int x = 10;    
    ValuePair vp5(x, "text"); // ValuePair<int, const char*>

    ValuePair vp6("abc", "def"s); // ValuePair<const char*, std::string>

    ValuePair vp7("abc", "def"); // ValuePair<std::string, std::string>
}

TEST_CASE("CTAD - spacial case")
{
    std::vector vec{1, 2, 3};  // std::vector<int>

    std::vector data1{vec};    // std::vector<int> - SPECIAL CASE - copy syntax
    static_assert(std::is_same_v<decltype(data1), std::vector<int>>);
    REQUIRE(vec == data1);

    std::vector data2{vec, vec};
    static_assert(std::is_same_v<decltype(data2), std::vector<std::vector<int>>>);
}

//////////////////////////////////
// CTAD + aggregates

template <typename T>
struct Aggregate1
{
    T value;
};

// explicit deduction guide for aggregate
template <typename T>
Aggregate1(T) -> Aggregate1<T>;

TEST_CASE("CTAD + aggregate")
{
    Aggregate1 agg1{1}; // Aggregate1<int>

    Aggregate1 agg2{"text"}; // Aggregate<const char*>
}

///////////////////////////////////////////////////////////////////
// CTAD + std library

TEST_CASE("CTAD - std::lib")
{
    SECTION("std::pair")
    {
        auto p1 = make_pair(1, 3.14);
        pair<int, string> p2(1, "text");

        pair p3{1, 3.14};
        auto p4 = pair("text", 42); 
    }

    SECTION("std::tuple")
    {
        tuple tpl1{1, 3.14, "text", "abc"s};

        tuple tpl2{tpl1}; // CTAD - special case with copy syntax

        tuple tp4 = pair{1, 3.14};
    }

    SECTION("std::optional")
    {
        std::optional<int> o1 = 42;

        std::optional o2 = 42; // std::optional<int>

        std::optional o3 = o2; // std::optional<int> - special case with copy syntax
    }

    SECTION("smart pointers")
    {
        std::unique_ptr<std::string> ptr1{new string{"text"}};

        std::shared_ptr sptr = std::move(ptr1);
        std::weak_ptr wptr = sptr;
    }

    SECTION("std::function")
    {
        std::function<void()> f1 = &foo;

        std::function f2 = &foo;

        std::function f3 = [](int x) { return 42; };
    }

    SECTION("std containers")
    {
        std::vector vec{1, 2, 3}; // std::vector<int>
        REQUIRE(vec == vector{1, 2, 3});

        int tab[5] = {1, 2, 3};
        vector data(begin(tab), end(tab));
        REQUIRE(data == vector{1, 2, 3, 0, 0});

        list lst(vec.begin(), vec.end());

        array arr = {1, 2, 3, 4}; // std::array<int, 4>
    }
}

template <auto N>
struct Value
{
    constexpr static auto value{N};
};

template <auto... Ns> struct ValueList { };

TEST_CASE("auto + template param")
{
    static_assert(Value<42>::value == 42);
    static_assert(Value<42u>::value == 42u);

    using HeterogenousContainer = ValueList<3, 42u, '5'>;
}

TEST_CASE("auto + {}")
{
    int x1(10);
    int x2{10};

    auto ax1(10); // int
    auto ax2{10}; // int

    auto ax3{1, 2, 3};
}