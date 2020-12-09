#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include "catch.hpp"

using namespace std;

std::vector<std::string_view> split_text(std::string&& text, string_view separators = " ,;") = delete;

std::vector<std::string_view> split_text(string_view text, string_view separators = " ,;" )
{
    std::vector<std::string_view> tokens;   

    auto pos1 = cbegin(text);

    while(pos1 != cend(text))
    {
        auto pos2 = std::find_first_of(pos1, cend(text), cbegin(separators), cend(separators));

        auto token_starts = &(*pos1);
        auto token_length = pos2 - pos1;

        tokens.emplace_back(token_starts, token_length);

        if (pos2 == cend(text))
            break;
        
        pos1 = std::next(pos2);
    }

    return tokens;
}

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
