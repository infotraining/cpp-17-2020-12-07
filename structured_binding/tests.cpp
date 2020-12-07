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

        auto [x, y] = tab;

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
            REQUIRE(result.second == true);
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