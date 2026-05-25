#include <catch2/catch_amalgamated.hpp>

#include <type_traits>

#include "common/geo_coordinate.h"
#include "common/map_coordinate.h"

using myapp::common::GeoCoordinate;
using myapp::common::MapCoordinate;

TEST_CASE("GeoCoordinate: コンストラクタとアクセサ", "[geo]")
{
    const GeoCoordinate g{0.5, 1.0, 100.0};
    REQUIRE(g.latitude() == Catch::Approx(0.5));
    REQUIRE(g.longitude() == Catch::Approx(1.0));
    REQUIRE(g.altitude() == Catch::Approx(100.0));
}

TEST_CASE("MapCoordinate: コンストラクタとアクセサ", "[map]")
{
    const MapCoordinate m{35.0, 139.0, 50.0};
    REQUIRE(m.latitude_deg() == Catch::Approx(35.0));
    REQUIRE(m.longitude_deg() == Catch::Approx(139.0));
    REQUIRE(m.altitude_m() == Catch::Approx(50.0));
}

// 型安全: rad と deg は別の型であり、暗黙変換できないこと。
// この static_assert は本クラス設計の中核なのでテストに含めておく。
static_assert(!std::is_convertible_v<GeoCoordinate, MapCoordinate>,
              "GeoCoordinate から MapCoordinate への暗黙変換は禁止");
static_assert(!std::is_convertible_v<MapCoordinate, GeoCoordinate>,
              "MapCoordinate から GeoCoordinate への暗黙変換は禁止");
static_assert(!std::is_constructible_v<MapCoordinate, GeoCoordinate>,
              "MapCoordinate を GeoCoordinate から構築できない");
static_assert(!std::is_constructible_v<GeoCoordinate, MapCoordinate>,
              "GeoCoordinate を MapCoordinate から構築できない");
