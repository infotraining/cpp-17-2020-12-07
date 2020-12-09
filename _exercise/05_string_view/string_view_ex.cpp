#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include "catch.hpp"

using namespace std;

TEST_CASE("split with spaces")
{
    string text = "one two three four";

    auto words = split_text(text);

    auto expected = {"one", "two", "three", "four"};

    REQUIRE(equal(begin(expected), end(expected), begin(words)));
}

TEST_CASE("split with commas")
{
    string text = "one,two,three,four";

    auto words = split_text(text);

    auto expected = {"one", "two", "three", "four"};

    REQUIRE(equal(begin(expected), end(expected), begin(words)));
}
