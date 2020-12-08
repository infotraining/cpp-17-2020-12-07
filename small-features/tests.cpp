#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

#include "catch.hpp"

using namespace std;

class Person
{
    inline static int id_gen = 0;

public:
    inline static constexpr std::string_view class_id = "Person";

    const int id = ++id_gen;
};
using namespace std;

TEST_CASE("test")
{
    Person p;
    REQUIRE(p.id == 1);

    Person other_p;
    REQUIRE(other_p.id == 2);
}

template <typename TContainer>
void print(string_view prefix, const TContainer& container)
{
    std::cout << prefix << ": [ ";
    for (const auto& item : container)
        std::cout << item << " ";
    std::cout << "]\n";
}

struct Aggregate1
{
    int a;
    double b;
    int coord[3];
    std::string name;

    void print() const
    {
        std::cout << "Aggregate1{" << a << ", " << b;
        std::cout << ", [ ";
        for (const auto& item : coord)
            std::cout << item << " ";
        std::cout << "], '" << name << "'}\n";
    }
};

TEST_CASE("aggreagate")
{
    SECTION("arrays")
    {
        int tab1[5];
        print("tab1", tab1);

        int tab2[5] = {1, 2, 3, 4, 5};
        print("tab2", tab2);

        int tab3[5] = {};
        print("tab3", tab3);

        int tab4[5] = {1, 2, 3};
        print("tab4", tab4);
    }

    SECTION("Aggregate1")
    {
        static_assert(std::is_aggregate_v<Aggregate1>);
        Aggregate1 agg1;
        agg1.print();

        Aggregate1 agg2 {};
        agg2.print();

        Aggregate1 agg3 {1, 3.14, {1, 2}, "abc"};
        agg3.print();

        Aggregate1 agg4 {1, 3.14};
        agg4.print();
    }

    static_assert(std::is_aggregate_v<std::array<int, 4>>);
}

struct Aggregate2 : std::string, Aggregate1
{
    std::vector<int> data;
};

TEST_CASE("Aggregates in C++17")
{
    Aggregate2 agg {{"abc"}, {1, 3.14, {1, 2, 3}, "text"}, {6, 7, 8}};

    REQUIRE(agg.size() == 3);
    agg.print();
    print("agg.data", agg.data);
}

struct Point
{
    int x, y, z;

    Point(int x, int y)
        : x(x)
        , y(y)
    {
    }
};

TEST_CASE("init syntax")
{
    SECTION("Cpp98")
    {
        //Point pt{1, 2}; // ERROR
        Point pt(1, 2); // OK
    }

    SECTION("Cpp11")
    {
        Point pt1 {1, 2}; // OK
        Point pt2(1, 2); // OK
    }

    // SECTION("Cpp20")
    // {
    //     Point pt(1, 2);
    // }
}

////////////////////////////////////////
// attributes

struct ErrorCode
{
    int err_code;
    const char* msg;
};

[[nodiscard]] ErrorCode open_file(const std::string& filename)
{
    [[maybe_unused]] int flag = 42;
    return {13, "Error#13"};
}

void step1() { }
void step2() { }
void step3() { }

enum Coffee
{
    espresso,
    americano [[deprecated]] = espresso
};

namespace [[deprecated]] LegacyCode
{
    namespace Lib::Ver_1_0
    {
        void f(int n)
        {
            switch (n)
            {
            case 1:
            case 2:
                step1();
                [[fallthrough]];
            case 3: // no warning on fallthrough
                step2();
                break;
            case 4:
                step3();
            }
        }
    }
}

TEST_CASE("attributes")
{
    Coffee c = americano;

    ErrorCode ec = open_file("bad_name");

    REQUIRE(ec.err_code == 13);

    LegacyCode::Lib::Ver_1_0::f(2);
}