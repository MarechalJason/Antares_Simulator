#!/bin/bash
#
# Copyright 2007-2026, RTE (https://www.rte-france.com)
# See AUTHORS.txt
# SPDX-License-Identifier: MPL-2.0
# This file is part of Antares-Simulator,
# Adequacy and Performance assessment for interconnected energy networks.
#
# Antares_Simulator is free software: you can redistribute it and/or modify
# it under the terms of the Mozilla Public Licence 2.0 as published by
# the Mozilla Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# Antares_Simulator is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# Mozilla Public Licence 2.0 for more details.
#
# You should have received a copy of the Mozilla Public Licence 2.0
# along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
#

set -e

echo "=========================================="
echo "Checking C++23 Compiler Support"
echo "=========================================="
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if compiler is available
if ! command -v ${CXX:-c++} &> /dev/null; then
    echo -e "${RED}✗ C++ compiler not found${NC}"
    echo "Please install GCC 12+, Clang 15+, or MSVC 2022+"
    exit 1
fi

COMPILER=${CXX:-c++}
echo "Compiler: $COMPILER"
$COMPILER --version | head -1
echo ""

# Create temporary test file
TEST_FILE=$(mktemp /tmp/cpp23_test_XXXXXX.cpp)
trap "rm -f $TEST_FILE" EXIT

# Test 1: Basic C++23 compilation
echo -n "Testing basic C++23 compilation... "
cat > $TEST_FILE << 'EOF'
int main() { return 0; }
EOF

if $COMPILER -std=c++23 -c $TEST_FILE -o /dev/null 2>/dev/null; then
    echo -e "${GREEN}✓ PASS${NC}"
else
    echo -e "${RED}✗ FAIL${NC}"
    echo "Your compiler does not support -std=c++23"
    exit 1
fi

# Test 2: std::expected
echo -n "Testing std::expected support... "
cat > $TEST_FILE << 'EOF'
#include <expected>
int main() {
    std::expected<int, int> e = 42;
    return e.has_value() ? 0 : 1;
}
EOF

if $COMPILER -std=c++23 $TEST_FILE -o /dev/null 2>/dev/null; then
    echo -e "${GREEN}✓ PASS${NC}"
else
    echo -e "${YELLOW}⚠ PARTIAL${NC} - std::expected not available"
    echo "  Consider upgrading to GCC 13+, Clang 16+, or MSVC 2022 17.4+"
fi

# Test 3: ranges::to
echo -n "Testing ranges::to support... "
cat > $TEST_FILE << 'EOF'
#include <ranges>
#include <vector>
int main() {
    std::vector<int> v = {1, 2, 3};
    auto r = v | std::views::filter([](int i){ return i > 1; }) | std::ranges::to<std::vector>();
    return 0;
}
EOF

if $COMPILER -std=c++23 $TEST_FILE -o /dev/null 2>/dev/null; then
    echo -e "${GREEN}✓ PASS${NC}"
else
    echo -e "${YELLOW}⚠ PARTIAL${NC} - ranges::to not available"
fi

# Test 4: views::zip
echo -n "Testing views::zip support... "
cat > $TEST_FILE << 'EOF'
#include <ranges>
#include <vector>
int main() {
    std::vector<int> v1 = {1, 2};
    std::vector<int> v2 = {3, 4};
    for (auto [a, b] : std::views::zip(v1, v2)) {}
    return 0;
}
EOF

if $COMPILER -std=c++23 $TEST_FILE -o /dev/null 2>/dev/null; then
    echo -e "${GREEN}✓ PASS${NC}"
else
    echo -e "${YELLOW}⚠ PARTIAL${NC} - views::zip not available"
fi

# Test 5: string::contains
echo -n "Testing string::contains support... "
cat > $TEST_FILE << 'EOF'
#include <string>
int main() {
    std::string s = "test";
    return s.contains("es") ? 0 : 1;
}
EOF

if $COMPILER -std=c++23 $TEST_FILE -o /dev/null 2>/dev/null; then
    echo -e "${GREEN}✓ PASS${NC}"
else
    echo -e "${YELLOW}⚠ PARTIAL${NC} - string::contains not available"
fi

# Test 6: optional monadic operations
echo -n "Testing optional::transform support... "
cat > $TEST_FILE << 'EOF'
#include <optional>
int main() {
    std::optional<int> o = 42;
    auto r = o.transform([](int i){ return i * 2; });
    return 0;
}
EOF

if $COMPILER -std=c++23 $TEST_FILE -o /dev/null 2>/dev/null; then
    echo -e "${GREEN}✓ PASS${NC}"
else
    echo -e "${YELLOW}⚠ PARTIAL${NC} - optional::transform not available"
fi

echo ""
echo "=========================================="
echo "Summary"
echo "=========================================="
echo ""
echo "Your compiler has basic C++23 support."
echo ""
echo "Recommended compiler versions:"
echo "  • GCC 13+ (full C++23 support)"
echo "  • Clang 16+ (full C++23 support)"
echo "  • MSVC 2022 17.4+ (full C++23 support)"
echo ""
echo "To compile Antares Simulator with C++23:"
echo "  cmake -S . -B build -DCMAKE_CXX_STANDARD=23"
echo "  cmake --build build"
echo ""

