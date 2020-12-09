#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>

#include "catch.hpp"

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////////

// TODO
template <typename TContainer, typename... TArgs>
auto matches(const TContainer& container, const TArgs&... args)
{
    return (... + std::count(begin(container), end(container), args));
}

TEST_CASE("matches - returns how many items is stored in a container")
{
    // Tip: use std::count() algorithm

    vector<int> v{1, 2, 3, 4, 5};

    REQUIRE(matches(v, 2, 5) == 2);
    REQUIRE(matches(v, 100, 200) == 0);
    REQUIRE(matches("abccdef", 'x', 'y', 'z') == 0);
    REQUIRE(matches("abccdef", 'a', 'c', 'f') == 4);
}

/////////////////////////////////////////////////////////////////////////////////////////////////

class Gadget
{
public:
    virtual std::string id() const { return "a"; }
    virtual ~Gadget() = default;
};

class SuperGadget : public Gadget
{
public:
    std::string id() const override
    {
        return "b";
    }
};


template<typename... TArgs>
auto make_vector(TArgs&&... args)
{
   using TItem = std::common_type_t<TArgs...>; 

   std::vector<TItem> output;
   output.reserve(sizeof...(args));
   
   (..., output.push_back(std::forward<TArgs>(args)));  // left fold expression with , operator

   return output;
}


TEST_CASE("make_vector - create vector from a list of arguments")
{
    // Tip: use std::common_type_t<Ts...> trait

    using namespace Catch::Matchers;

    SECTION("ints")
    {
        std::vector<int> v = make_vector(1, 2, 3);

        REQUIRE_THAT(v, Equals(vector{1, 2, 3}));
    }

    SECTION("unique_ptrs")
    {
        auto ptrs = make_vector(make_unique<int>(5), make_unique<int>(6));

        REQUIRE(ptrs.size() == 2);
    }

    SECTION("unique_ptrs with polymorphic hierarchy")
    {
        auto gadgets = make_vector(make_unique<Gadget>(), make_unique<SuperGadget>(), make_unique<Gadget>());

        static_assert(is_same_v<decltype(gadgets)::value_type, unique_ptr<Gadget>>);

        vector<string> ids;
        transform(begin(gadgets), end(gadgets), back_inserter(ids), [](auto& ptr) { return ptr->id(); });

        REQUIRE_THAT(ids, Equals(vector<string>{"a", "b", "a"}));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct Range
{
    T low, high;    

    bool operator()(const T& value) const
    {
        return (low <= value) && (value <= high);
    }
};

template <typename T1, typename T2>
Range(T1, T2) -> Range<std::common_type_t<T1, T2>>;

template <typename TRange, typename... TArgs>
bool within(const TRange& in_range, const TArgs&... args)
{
    return (... && in_range(args));
}

TEST_CASE("functor Range")
{
    Range in_range{10, 20};
    REQUIRE(in_range(12));
}

TEST_CASE("within - checks if all values fit in range [low, high]")
{
    REQUIRE(within(Range{10, 20.1}, 1, 15, 30) == false);
    REQUIRE(within(Range{10, 20}, 11, 12, 13) == true);
    REQUIRE(within(Range{5.0, 5.5}, 5.1, 5.2, 5.3) == true);
}

// /////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void hash_combine(size_t& seed, const T& value)
{
    seed ^= hash<T>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <typename... TArgs>
size_t combined_hash(const TArgs&... args)
{
    size_t seed{};
    (..., hash_combine(seed, args));
    
    return seed;
}

TEST_CASE("combined_hash - write a function that calculates combined hash value for a given number of arguments")
{
    REQUIRE(combined_hash(1U) == 2654435770U);
    REQUIRE(combined_hash(1, 3.14, "string"s) == 10365827363824479057U);
    REQUIRE(combined_hash(123L, "abc"sv, 234, 3.14f) == 162170636579575197U);
}