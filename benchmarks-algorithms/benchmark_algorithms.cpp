#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <cmath>
#include <execution>
#include <fstream>
#include <iostream>
#include <optional>
#include <random>
#include <string>
#include <thread>

using DocumentContent = std::vector<std::string>;

std::optional<DocumentContent> load_words(const std::string &file_name)
{
    std::ifstream input_file{file_name};

    if (!input_file)
        return std::nullopt;

    DocumentContent words;

    for (std::string token; input_file >> token;)
    {
        words.push_back(token);
    }

    return words;
}

inline const DocumentContent words = [] { DocumentContent words = load_words("tokens.txt").value(); words.resize(words.size() / 10);  return words; }();

TEST_CASE("hardware concurrency")
{
    std::cout << "No of cores: " << std::thread::hardware_concurrency() << "\n";
    std::cout << "No of words: " << words.size() << std::endl;
}

TEST_CASE("accumulate")
{
    auto calc_hash = [](const auto &item) { return std::hash<std::remove_cv_t<std::remove_reference_t<decltype(item)>>>{}(item); };

    BENCHMARK("std::accumulate")
    {
        return std::accumulate(words.begin(), words.end(), 0ULL, [=](const auto &total, const auto &word) { return total + calc_hash(word);; });
    };

    BENCHMARK("std::transform_reduce - parallel")
    {
        return std::transform_reduce(std::execution::par, words.begin(), words.end(), 0ULL, std::plus{}, calc_hash);
    };

    BENCHMARK("std::transform_reduce - parallel unsequenced")
    {
        return std::transform_reduce(std::execution::par, words.begin(), words.end(), 0ULL, std::plus{}, calc_hash);
    };
}

TEST_CASE("sort")
{
    BENCHMARK_ADVANCED("sequenced")
    (Catch::Benchmark::Chronometer meter)
    {
        auto words_to_sort = words;
        REQUIRE_FALSE(std::is_sorted(words_to_sort.begin(), words_to_sort.end()));

        meter.measure([&] {
            std::sort(
                words_to_sort.begin(), words_to_sort.end(),
                [](const auto &a, const auto &b) { return boost::to_lower_copy(a) < boost::to_lower_copy(b); });
            return words_to_sort.front();
        });
    };

    BENCHMARK_ADVANCED("parallel")
    (Catch::Benchmark::Chronometer meter)
    {
        auto words_to_sort = words;
        REQUIRE_FALSE(std::is_sorted(words_to_sort.begin(), words_to_sort.end()));

        meter.measure([&] {
            std::sort(
                std::execution::par,
                words_to_sort.begin(), words_to_sort.end(),
                [](const auto &a, const auto &b) { return boost::to_lower_copy(a) < boost::to_lower_copy(b); });

            return words_to_sort.front();
        });
    };

    BENCHMARK_ADVANCED("parallel unsequenced")
    (Catch::Benchmark::Chronometer meter)
    {
        auto words_to_sort = words;
        REQUIRE_FALSE(std::is_sorted(words_to_sort.begin(), words_to_sort.end()));

        meter.measure([&] {
            std::for_each(std::execution::par, words_to_sort.begin(), words_to_sort.end(), [](auto &w) { boost::to_lower(w); });
            std::vector<std::string_view> words_views(words_to_sort.size());
            std::transform(std::execution::par, words_to_sort.begin(), words_to_sort.end(), words_views.begin(), [](const auto &w) { return std::string_view(w); });

            std::sort(
                std::execution::par_unseq,
                words_views.begin(), words_views.end());

            return std::string(words_views.front());
        });
    };
}

bool is_prime(uint64_t number)
{
    if (number < 2)
    {
        return false;
    }
    else if (number % 2 == 0 && number != 2)
    {
        return false;
    }
    else
    {
        for (uint64_t i = 3; i <= sqrt(number); i += 2)
        {
            if (number % i == 0)
                return false;
        }
        return true;
    }
}

const size_t no_of_items = 20'000;

const std::vector<uint64_t> numbers = [] {
    std::random_device rd;
    std::mt19937_64 rnd_gen{rd()};
    std::uniform_int_distribution<uint64_t> rnd_distr(0, no_of_items);

    std::vector<uint64_t> numbers(no_of_items);
    std::generate(numbers.begin(), numbers.end(), [&] { return rnd_distr(rnd_gen); });

    return numbers;
}();

TEST_CASE("transform")
{
    BENCHMARK_ADVANCED("sequenced")
    (Catch::Benchmark::Chronometer meter)
    {
        auto numbers_to_part = numbers;
        decltype(numbers_to_part) are_primes(numbers_to_part.size());

        meter.measure([&] {
            std::transform(numbers_to_part.begin(), numbers_to_part.end(), are_primes.begin(), [](auto n) { return is_prime(n); });
            return are_primes;
        });
    };

    BENCHMARK_ADVANCED("parallel")
    (Catch::Benchmark::Chronometer meter)
    {
        auto numbers_to_part = numbers;
        decltype(numbers_to_part) are_primes(numbers_to_part.size());

        meter.measure([&] {
            std::transform(std::execution::par_unseq, numbers_to_part.begin(), numbers_to_part.end(), are_primes.begin(), [](auto n) { return is_prime(n); });
            return are_primes;
        });
    };
}

TEST_CASE("partition")
{
    BENCHMARK_ADVANCED("sequenced")
    (Catch::Benchmark::Chronometer meter)
    {
        auto numbers_to_part = numbers;

        meter.measure([&] {
            return std::partition(numbers_to_part.begin(), numbers_to_part.end(), [](auto n) { return is_prime(n); });
        });
    };

    BENCHMARK_ADVANCED("parallel unsequencedszkol")
    (Catch::Benchmark::Chronometer meter)
    {
        auto numbers_to_part = numbers;

        meter.measure([&] {
            return std::partition(std::execution::par_unseq, numbers_to_part.begin(), numbers_to_part.end(), [](auto n) { return is_prime(n); });
        });
    };
}