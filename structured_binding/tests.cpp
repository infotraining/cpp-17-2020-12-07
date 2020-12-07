#include <algorithm>
#include <array>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "catch.hpp"

using namespace std;

namespace BeforeCpp17
{
    tuple<int, int, double> calc_stats(const vector<int>& data)
    {
        vector<int>::const_iterator min_pos, max_pos;
        tie(min_pos, max_pos) = minmax_element(data.begin(), data.end());

        double avg = accumulate(data.begin(), data.end(), 0.0) / data.size();

        return make_tuple(*min_pos, *max_pos, avg);
    }
}

template <typename TContainer>
std::tuple<int, int, double> calc_stats(const TContainer& data)
{
    auto [min_pos, max_pos] = std::minmax_element(std::begin(data), std::end(data));

    double avg = std::accumulate(std::begin(data), std::end(data), 0.0) / std::size(data);

    return std::tuple(*min_pos, *max_pos, avg);
}

TEST_CASE("Before C++17")
{
    int data[] = {4, 42, 665, 1, 123, 13};

    SECTION("C++11")
    {
        auto results = calc_stats(data);
        REQUIRE(std::get<0>(results) == 1); // min

        int min, max;
        double avg;

        tie(min, max, avg) = calc_stats(data);

        REQUIRE(min == 1);
        REQUIRE(max == 665);
        REQUIRE(avg == Approx(141.333));

        tie(min, max, std::ignore) = calc_stats(data);
    }

    SECTION("C++17")
    {
        auto [min, max, avg] = calc_stats(data);

        REQUIRE(min == 1);
        REQUIRE(max == 665);
        REQUIRE(avg == Approx(141.333));
    }
}

std::array<int, 3> get_coord()
{
    return array {1, 2, 3};
}

struct ErrorCode
{
    int ec;
    const char* m;
};

ErrorCode open_file(const char* filename)
{
    return ErrorCode {13, "Error#13"};
}

TEST_CASE("structured bindings - details")
{
    SECTION("native arrays")
    {
        int tab[2] = {1, 2};

        auto& [x, y] = tab;

        REQUIRE(x == 1);
        REQUIRE(y == 2);
    }

    SECTION("std::array")
    {
        auto [x, y, z] = get_coord();

        REQUIRE(x == 1);
        REQUIRE(y == 2);
        REQUIRE(z == 3);
    }

    SECTION("std::pair")
    {
        std::set numbers = {1, 4, 5};

        SECTION("before C++17")
        {
            std::set<int>::iterator pos;
            bool was_inserted;
            std::tie(pos, was_inserted) = numbers.insert(2);

            auto result = numbers.insert(2);
            REQUIRE(*result.first == 2);
            REQUIRE(result.second == false);
        }

        SECTION("since C++17")
        {
            auto [pos, was_inserted] = numbers.insert(2);

            REQUIRE(*pos == 2);
            REQUIRE(was_inserted);
        }
    }

    SECTION("std::tuple")
    {
        auto [n, pi, str] = tuple(1, 3.14, "text"s);

        REQUIRE(n == 1);
        REQUIRE(str == "text"s);
    }

    SECTION("struct/classes")
    {
        auto [error_code, message] = open_file("wrong_path");

        REQUIRE(error_code == 13);
    }
}

struct Timestamp
{
    int h, m, s;
};

TEST_CASE("structured bindings - how it works")
{
    Timestamp t1 {12, 50, 0};

    auto& [hours, minutes, seconds] = t1;

    hours = 13;

    int& meeting_hour = hours;

    meeting_hour = 15;

    REQUIRE(t1.h == 15);

    SECTION("is interpreted as")
    {
        const auto& entity = t1;

        auto& hours = t1.h;
        auto& minutes = t1.m;
        auto& seconds = t1.s;
    }
}

struct Person
{
    string fname, lname;
};

TEST_CASE("structured binding - move semantics")
{
    Person participant {"John", "Smith"};

    SECTION("transfer of state to anonymous entity")
    {
        auto [first_name, last_name] = std::move(participant);

        REQUIRE(participant.fname == ""s);
        REQUIRE(participant.lname == ""s);
        REQUIRE(first_name == "John"s);
    }

    SECTION("is intepreted")
    {
        auto entity = std::move(participant);
        auto& first_name = entity.fname;
        auto& last_name = entity.lname;
    }

    SECTION("&& - only static cast")
    {
        auto&& [first_name, last_name] = std::move(participant);

        REQUIRE(participant.fname == "John"s);
        REQUIRE(participant.lname == "Smith"s);
        REQUIRE(first_name == "John"s);
    }
}

TEST_CASE("use cases")
{
    SECTION("iteration over map")
    {
        std::map<int, std::string> dict = { {1, "one"s}, {2, "two"}, {3, "three"} };

        for(const auto& [key, value] : dict)
        {
            std::cout << key << " - " << value << "\n";
        }       
    }

    SECTION("multiple initialization")
    {
        set numbers = {1, 2, 3};

        for(auto [index, it] = std::tuple(0, begin(numbers)); it != end(numbers); ++it, ++index)
        {
            std::cout << index << " - " << *it << "\n";
        }
    }
}

///////////////////////////////////////////////////////////
// tuple like protocol

enum Something
{
    some = 1,
    thing
};

const std::map<Something, std::string_view> something_desc = {
    { some, "some"sv}, {thing, "thing"sv}
};

// step 1 - std::tuple_size<Something>
template <>
struct std::tuple_size<Something>
{
    static constexpr size_t value = 2;
};

// step 2 - std::tuple_element<i, Something>
template <>
struct std::tuple_element<0, Something>
{
    using type = int;
};

template <>
struct std::tuple_element<1, Something>
{
    using type = std::string_view;
};

// step 3 - get<Index>
template <size_t Index>
decltype(auto) get(const Something&);

template <>
decltype(auto) get<0>(const Something& sth)
{
    return static_cast<int>(sth);
}

template <>
decltype(auto) get<1>(const Something& sth)
{
    return something_desc.at(sth);
}

TEST_CASE("tuple like protocol")
{
    Something sth = some;

    const auto [value, description] = sth;

    REQUIRE(value == 1);
    REQUIRE(description == "some"sv);

    sth = thing;

    const auto [v, desc] = sth;
    REQUIRE(v == 2);
    REQUIRE(desc == "thing"sv);
}