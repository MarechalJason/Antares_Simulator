/*
 * Copyright 2007-2026, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#define BOOST_TEST_MODULE test cpp23 features
#include <algorithm>
#include <expected>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

#include <boost/test/unit_test.hpp>

namespace ranges = std::ranges;
namespace views = std::views;

BOOST_AUTO_TEST_SUITE(cpp23_features)

// ============================================================================
// std::expected Tests
// ============================================================================

enum class ErrorCode
{
    FileNotFound,
    PermissionDenied,
    InvalidFormat,
    UnknownError
};

struct FileContent
{
    std::string data;
    size_t size;
};

// Helper function that returns expected
std::expected<FileContent, ErrorCode> readFile(std::string_view filename)
{
    if (filename.empty())
    {
        return std::unexpected(ErrorCode::FileNotFound);
    }

    if (filename == "forbidden.txt")
    {
        return std::unexpected(ErrorCode::PermissionDenied);
    }

    if (filename == "invalid.bin")
    {
        return std::unexpected(ErrorCode::InvalidFormat);
    }

    // Success case
    return FileContent{std::string(filename), filename.size()};
}

BOOST_AUTO_TEST_CASE(test_expected_success)
{
    auto result = readFile("test.txt");

    BOOST_CHECK(result.has_value());
    BOOST_CHECK_EQUAL(result.value().data, "test.txt");
    BOOST_CHECK_EQUAL(result.value().size, 8);
}

BOOST_AUTO_TEST_CASE(test_expected_error)
{
    auto result = readFile("");

    BOOST_CHECK(!result.has_value());
    BOOST_CHECK(result.error() == ErrorCode::FileNotFound);
}

BOOST_AUTO_TEST_CASE(test_expected_value_or)
{
    auto result = readFile("forbidden.txt");

    FileContent defaultContent{"default", 7};
    auto content = result.value_or(defaultContent);

    BOOST_CHECK_EQUAL(content.data, "default");
    BOOST_CHECK_EQUAL(content.size, 7);
}

BOOST_AUTO_TEST_CASE(test_expected_and_then)
{
    auto processFile = [](const FileContent& content) -> std::expected<int, ErrorCode>
    {
        if (content.data.empty())
        {
            return std::unexpected(ErrorCode::InvalidFormat);
        }
        return static_cast<int>(content.size * 2);
    };

    auto result = readFile("test.txt").and_then(processFile);

    BOOST_CHECK(result.has_value());
    BOOST_CHECK_EQUAL(result.value(), 16);
}

BOOST_AUTO_TEST_CASE(test_expected_or_else)
{
    auto handleError = [](ErrorCode error) -> std::expected<FileContent, ErrorCode>
    {
        if (error == ErrorCode::FileNotFound)
        {
            return FileContent{"recovered", 9};
        }
        return std::unexpected(error);
    };

    auto result = readFile("").or_else(handleError);

    BOOST_CHECK(result.has_value());
    BOOST_CHECK_EQUAL(result.value().data, "recovered");
}

BOOST_AUTO_TEST_CASE(test_expected_transform)
{
    auto extractSize = [](const FileContent& content) { return content.size; };

    auto result = readFile("test.txt").transform(extractSize);

    BOOST_CHECK(result.has_value());
    BOOST_CHECK_EQUAL(result.value(), 8);
}

// ============================================================================
// C++23 Ranges Enhancements Tests
// ============================================================================

BOOST_AUTO_TEST_CASE(test_ranges_views_chunk)
{
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    std::vector<std::vector<int>> chunks;

    for (auto chunk: numbers | views::chunk(3))
    {
    }

    BOOST_CHECK_EQUAL(chunks.size(), 3);
    BOOST_CHECK_EQUAL(chunks[0].size(), 3);
    BOOST_CHECK_EQUAL(chunks[1].size(), 3);
    BOOST_CHECK_EQUAL(chunks[2].size(), 3);

    BOOST_CHECK_EQUAL(chunks[0][0], 1);
    BOOST_CHECK_EQUAL(chunks[1][0], 4);
    BOOST_CHECK_EQUAL(chunks[2][0], 7);
}

BOOST_AUTO_TEST_CASE(test_ranges_views_slide)
{
    std::vector<int> numbers = {1, 2, 3, 4, 5};

    std::vector<std::vector<int>> windows;

    for (auto window: numbers | views::slide(3))
    {
    }

    BOOST_CHECK_EQUAL(windows.size(), 3);

    std::vector<int> expected_first = {1, 2, 3};
    std::vector<int> expected_second = {2, 3, 4};
    std::vector<int> expected_third = {3, 4, 5};

    BOOST_CHECK_EQUAL_COLLECTIONS(windows[0].begin(),
                                  windows[0].end(),
                                  expected_first.begin(),
                                  expected_first.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(windows[1].begin(),
                                  windows[1].end(),
                                  expected_second.begin(),
                                  expected_second.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(windows[2].begin(),
                                  windows[2].end(),
                                  expected_third.begin(),
                                  expected_third.end());
}

// ============================================================================
// C++23 <ranges> with std::expected Integration
// ============================================================================

BOOST_AUTO_TEST_CASE(test_expected_with_ranges)
{
    std::vector<std::string> filenames = {"file1.txt", "", "file2.txt", "forbidden.txt"};

    // Process files and collect successful results
    std::vector<FileContent> successfulReads;
    std::vector<ErrorCode> errors;

    for (const auto& filename: filenames)
    {
        auto result = readFile(filename);
        if (result.has_value())
        {
            successfulReads.push_back(result.value());
        }
        else
        {
            errors.push_back(result.error());
        }
    }

    BOOST_CHECK_EQUAL(successfulReads.size(), 2);
    BOOST_CHECK_EQUAL(errors.size(), 2);

    BOOST_CHECK_EQUAL(successfulReads[0].data, "file1.txt");
    BOOST_CHECK_EQUAL(successfulReads[1].data, "file2.txt");

    BOOST_CHECK(errors[0] == ErrorCode::FileNotFound);
    BOOST_CHECK(errors[1] == ErrorCode::PermissionDenied);
}

// ============================================================================
// C++23 constexpr Enhancements
// ============================================================================

constexpr int factorial(int n)
{
    if (n <= 1)
    {
        return 1;
    }
    return n * factorial(n - 1);
}

BOOST_AUTO_TEST_CASE(test_constexpr_calculations)
{
    constexpr int fact5 = factorial(5);
    static_assert(fact5 == 120, "Factorial calculation failed");

    BOOST_CHECK_EQUAL(fact5, 120);
    BOOST_CHECK_EQUAL(factorial(6), 720);
}

// ============================================================================
// std::optional with C++23 monadic operations
// ============================================================================

BOOST_AUTO_TEST_CASE(test_optional_and_then)
{
    std::optional<int> value = 42;

    auto result = value.and_then([](int v) -> std::optional<int> { return v * 2; });

    BOOST_CHECK(result.has_value());
    BOOST_CHECK_EQUAL(result.value(), 84);
}

BOOST_AUTO_TEST_CASE(test_optional_transform)
{
    std::optional<std::string> str = "hello";

    auto result = str.transform([](const std::string& s) { return s.size(); });

    BOOST_CHECK(result.has_value());
    BOOST_CHECK_EQUAL(result.value(), 5);
}

BOOST_AUTO_TEST_CASE(test_optional_or_else)
{
    std::optional<int> empty;

    auto result = empty.or_else([]() -> std::optional<int> { return 100; });

    BOOST_CHECK(result.has_value());
    BOOST_CHECK_EQUAL(result.value(), 100);
}

// ============================================================================
// String Literal Suffixes and contains()
// ============================================================================

BOOST_AUTO_TEST_CASE(test_string_contains)
{
    using namespace std::string_literals;

    std::string text = "Antares Simulator C++23"s;

    BOOST_CHECK(text.contains("Antares"));
    BOOST_CHECK(text.contains("C++23"));
    BOOST_CHECK(!text.contains("Python"));

    std::string_view view = text;
    BOOST_CHECK(view.contains("Simulator"));
}

BOOST_AUTO_TEST_SUITE_END()
