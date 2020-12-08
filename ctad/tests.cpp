#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <memory>

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