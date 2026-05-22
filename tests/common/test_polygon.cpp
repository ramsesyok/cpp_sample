#include <catch2/catch_amalgamated.hpp>

#include "common/polygon.h"

using myapp::common::Point;
using myapp::common::Polygon;
using myapp::common::contains;

TEST_CASE("contains: 正方形", "[polygon]") {
    const Polygon square = {{0.0, 0.0}, {10.0, 0.0}, {10.0, 10.0}, {0.0, 10.0}};

    SECTION("内部の点は true") {
        REQUIRE(contains(square, Point{5.0, 5.0}));
    }
    SECTION("外部の点は false") {
        REQUIRE_FALSE(contains(square, Point{15.0, 5.0}));
        REQUIRE_FALSE(contains(square, Point{-1.0, 5.0}));
    }
}

TEST_CASE("contains: 凹多角形 (L字)", "[polygon]") {
    const Polygon l_shape = {
        {0.0, 0.0}, {4.0, 0.0}, {4.0, 2.0},
        {2.0, 2.0}, {2.0, 4.0}, {0.0, 4.0},
    };

    REQUIRE(contains(l_shape, Point{1.0, 1.0}));
    REQUIRE(contains(l_shape, Point{1.0, 3.0}));
    REQUIRE_FALSE(contains(l_shape, Point{3.0, 3.0}));
}

TEST_CASE("contains: 縮退ケース", "[polygon]") {
    REQUIRE_FALSE(contains(Polygon{}, Point{0.0, 0.0}));
    REQUIRE_FALSE(contains(Polygon{{0.0, 0.0}, {1.0, 1.0}}, Point{0.5, 0.5}));
}
