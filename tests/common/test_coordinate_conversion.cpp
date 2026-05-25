#include <catch2/catch_amalgamated.hpp>

#include <cmath>

#include "common/coordinate_conversion.h"
#include "common/ecef_position.h"

using myapp::common::ecef_to_geo;
using myapp::common::EcefPosition;
using myapp::common::geo_to_ecef;
using myapp::common::GeoCoordinate;
using myapp::common::MapCoordinate;
using myapp::common::to_geo;
using myapp::common::to_map;

TEST_CASE("to_map: ラジアン -> 度", "[coord_conv]")
{
    // pi/4 rad = 45 deg
    const GeoCoordinate g{0.7853981633974483, 1.5707963267948966, 123.0};
    const MapCoordinate m = to_map(g);
    REQUIRE(m.latitude_deg() == Catch::Approx(45.0));
    REQUIRE(m.longitude_deg() == Catch::Approx(90.0));
    REQUIRE(m.altitude_m() == Catch::Approx(123.0));
}

TEST_CASE("to_geo: 度 -> ラジアン", "[coord_conv]")
{
    const MapCoordinate m{180.0, -90.0, 50.0};
    const GeoCoordinate g = to_geo(m);
    REQUIRE(g.latitude() == Catch::Approx(3.141592653589793));
    REQUIRE(g.longitude() == Catch::Approx(-1.5707963267948966));
    REQUIRE(g.altitude() == Catch::Approx(50.0));
}

TEST_CASE("geo_to_ecef: 赤道・本初子午線・標高0", "[coord_conv][ecef]")
{
    // φ=0, λ=0, h=0 -> (a, 0, 0)
    const GeoCoordinate g{0.0, 0.0, 0.0};
    const EcefPosition p = geo_to_ecef(g);
    REQUIRE(p.x() == Catch::Approx(6378137.0));
    REQUIRE(p.y() == Catch::Approx(0.0).margin(1e-6));
    REQUIRE(p.z() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("geo_to_ecef: 赤道・東経90度・標高0", "[coord_conv][ecef]")
{
    // φ=0, λ=π/2 -> (0, a, 0)
    const GeoCoordinate g{0.0, 1.5707963267948966, 0.0};
    const EcefPosition p = geo_to_ecef(g);
    REQUIRE(p.x() == Catch::Approx(0.0).margin(1e-6));
    REQUIRE(p.y() == Catch::Approx(6378137.0));
    REQUIRE(p.z() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("geo_to_ecef: 北極", "[coord_conv][ecef]")
{
    // φ=π/2 -> (0, 0, b)  ここで b = a*(1-f) ≒ 6356752.3142 (WGS84 短半径)
    const GeoCoordinate g{1.5707963267948966, 0.0, 0.0};
    const EcefPosition p = geo_to_ecef(g);
    REQUIRE(p.x() == Catch::Approx(0.0).margin(1e-6));
    REQUIRE(p.y() == Catch::Approx(0.0).margin(1e-6));
    REQUIRE(p.z() == Catch::Approx(6356752.314245179));
}

TEST_CASE("geo_to_ecef: 標高が直接 X に加算される (赤道)", "[coord_conv][ecef]")
{
    // φ=0, λ=0, h=1000 -> (a+1000, 0, 0)
    const GeoCoordinate g{0.0, 0.0, 1000.0};
    const EcefPosition p = geo_to_ecef(g);
    REQUIRE(p.x() == Catch::Approx(6378137.0 + 1000.0));
    REQUIRE(p.y() == Catch::Approx(0.0).margin(1e-6));
    REQUIRE(p.z() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("geo_to_ecef: 原点からの距離が a に近い (赤道)", "[coord_conv][ecef]")
{
    // 赤道上の任意の経度で、原点からの距離は a (= 6378137 m) になるはず。
    // ハードコードした参考値に依存せず、不変量で検証する。
    const GeoCoordinate g{0.0, 2.5, 0.0}; // 経度は適当な値
    const EcefPosition p = geo_to_ecef(g);
    const double dist = std::sqrt(p.x() * p.x() + p.y() * p.y() + p.z() * p.z());
    REQUIRE(dist == Catch::Approx(6378137.0));
    REQUIRE(p.z() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("変換ラウンドトリップ", "[coord_conv]")
{
    const MapCoordinate original{35.6812, 139.7671, 40.0}; // 東京駅近辺
    const MapCoordinate round = to_map(to_geo(original));
    REQUIRE(round.latitude_deg() == Catch::Approx(original.latitude_deg()));
    REQUIRE(round.longitude_deg() == Catch::Approx(original.longitude_deg()));
    REQUIRE(round.altitude_m() == Catch::Approx(original.altitude_m()));
}

TEST_CASE("ecef_to_geo: 赤道・本初子午線", "[coord_conv][ecef]")
{
    // (a, 0, 0) -> (φ=0, λ=0, h=0)
    const GeoCoordinate g = ecef_to_geo(EcefPosition{6378137.0, 0.0, 0.0});
    REQUIRE(g.latitude() == Catch::Approx(0.0).margin(1e-12));
    REQUIRE(g.longitude() == Catch::Approx(0.0).margin(1e-12));
    REQUIRE(g.altitude() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("ecef_to_geo: 赤道・東経90度", "[coord_conv][ecef]")
{
    // (0, a, 0) -> (φ=0, λ=π/2, h=0)
    const GeoCoordinate g = ecef_to_geo(EcefPosition{0.0, 6378137.0, 0.0});
    REQUIRE(g.latitude() == Catch::Approx(0.0).margin(1e-12));
    REQUIRE(g.longitude() == Catch::Approx(1.5707963267948966));
    REQUIRE(g.altitude() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("ecef_to_geo: 北極 (極近傍ガード経路)", "[coord_conv][ecef]")
{
    // (0, 0, b) -> (φ=π/2, λ=任意 (本実装では 0), h=0)
    // b = a(1-f) ≒ 6356752.3142
    const GeoCoordinate g = ecef_to_geo(EcefPosition{0.0, 0.0, 6356752.314245179});
    REQUIRE(g.latitude() == Catch::Approx(1.5707963267948966));
    REQUIRE(g.altitude() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("ecef_to_geo: 南極上空 1000m", "[coord_conv][ecef]")
{
    // 南極の真上、楕円体高 1000m。Z = -(b + 1000)、X=Y=0 の自転軸上ケース。
    const GeoCoordinate g = ecef_to_geo(EcefPosition{0.0, 0.0, -(6356752.314245179 + 1000.0)});
    REQUIRE(g.latitude() == Catch::Approx(-1.5707963267948966));
    REQUIRE(g.altitude() == Catch::Approx(1000.0).margin(1e-6));
}

TEST_CASE("ecef <-> geo ラウンドトリップ (各種地点)", "[coord_conv][ecef]")
{
    // 様々な (緯度, 経度, 高度) を geo -> ecef -> geo して元に戻ることを確認する。
    // Bowring 法は地表近傍で機械精度に収束するので、margin/epsilon は厳しく取ってよい。
    struct Case
    {
        double lat_deg;
        double lon_deg;
        double alt_m;
    };
    const Case cases[] = {
        {35.6812, 139.7671, 40.0},     // 東京
        {-33.8688, 151.2093, 58.0},    // シドニー
        {64.1466, -21.9426, 0.0},      // レイキャビク
        {-1.2921, 36.8219, 1795.0},    // ナイロビ (標高あり)
        {45.0, 0.0, 10000.0},          // 北緯45° 高高度
        {0.0, 180.0, 0.0},             // 日付変更線
    };

    for (const auto &c : cases)
    {
        const GeoCoordinate original = to_geo(MapCoordinate{c.lat_deg, c.lon_deg, c.alt_m});
        const GeoCoordinate round = ecef_to_geo(geo_to_ecef(original));
        INFO("case lat_deg=" << c.lat_deg << " lon_deg=" << c.lon_deg << " alt_m=" << c.alt_m);
        REQUIRE(round.latitude() == Catch::Approx(original.latitude()).margin(1e-10));
        REQUIRE(round.longitude() == Catch::Approx(original.longitude()).margin(1e-10));
        REQUIRE(round.altitude() == Catch::Approx(original.altitude()).margin(1e-6));
    }
}
