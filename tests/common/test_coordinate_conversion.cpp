#include <catch2/catch_amalgamated.hpp>
#include <cmath>

#include "common/coordinate_conversion.h"
#include "common/ecef_coordinate.h"

using myapp::common::EcefCoordinate;
using myapp::common::EcefToGeo;
using myapp::common::GeoCoordinate;
using myapp::common::GeoToEcef;
using myapp::common::MapCoordinate;
using myapp::common::ToGeo;
using myapp::common::ToMap;

TEST_CASE("ToMap: ラジアン -> 度", "[coord_conv]") {
  // pi/4 rad = 45 deg
  const GeoCoordinate kG{0.7853981633974483, 1.5707963267948966, 123.0};
  const MapCoordinate kM = ToMap(kG);
  REQUIRE(kM.LatitudeDeg() == Catch::Approx(45.0));
  REQUIRE(kM.LongitudeDeg() == Catch::Approx(90.0));
  REQUIRE(kM.AltitudeM() == Catch::Approx(123.0));
}

TEST_CASE("ToGeo: 度 -> ラジアン", "[coord_conv]") {
  const MapCoordinate kM{180.0, -90.0, 50.0};
  const GeoCoordinate kG = ToGeo(kM);
  REQUIRE(kG.Latitude() == Catch::Approx(3.141592653589793));
  REQUIRE(kG.Longitude() == Catch::Approx(-1.5707963267948966));
  REQUIRE(kG.Altitude() == Catch::Approx(50.0));
}

TEST_CASE("GeoToEcef: 赤道・本初子午線・標高0", "[coord_conv][ecef]") {
  // φ=0, λ=0, h=0 -> (a, 0, 0)
  const GeoCoordinate kG{0.0, 0.0, 0.0};
  const EcefCoordinate kP = GeoToEcef(kG);
  REQUIRE(kP.X() == Catch::Approx(6378137.0));
  REQUIRE(kP.Y() == Catch::Approx(0.0).margin(1e-6));
  REQUIRE(kP.Z() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("GeoToEcef: 赤道・東経90度・標高0", "[coord_conv][ecef]") {
  // φ=0, λ=π/2 -> (0, a, 0)
  const GeoCoordinate kG{0.0, 1.5707963267948966, 0.0};
  const EcefCoordinate kP = GeoToEcef(kG);
  REQUIRE(kP.X() == Catch::Approx(0.0).margin(1e-6));
  REQUIRE(kP.Y() == Catch::Approx(6378137.0));
  REQUIRE(kP.Z() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("GeoToEcef: 北極", "[coord_conv][ecef]") {
  // φ=π/2 -> (0, 0, b)  ここで b = a*(1-f) ≒ 6356752.3142 (WGS84 短半径)
  const GeoCoordinate kG{1.5707963267948966, 0.0, 0.0};
  const EcefCoordinate kP = GeoToEcef(kG);
  REQUIRE(kP.X() == Catch::Approx(0.0).margin(1e-6));
  REQUIRE(kP.Y() == Catch::Approx(0.0).margin(1e-6));
  REQUIRE(kP.Z() == Catch::Approx(6356752.314245179));
}

TEST_CASE("GeoToEcef: 標高が直接 X に加算される (赤道)", "[coord_conv][ecef]") {
  // φ=0, λ=0, h=1000 -> (a+1000, 0, 0)
  const GeoCoordinate kG{0.0, 0.0, 1000.0};
  const EcefCoordinate kP = GeoToEcef(kG);
  REQUIRE(kP.X() == Catch::Approx(6378137.0 + 1000.0));
  REQUIRE(kP.Y() == Catch::Approx(0.0).margin(1e-6));
  REQUIRE(kP.Z() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("GeoToEcef: 原点からの距離が a に近い (赤道)", "[coord_conv][ecef]") {
  // 赤道上の任意の経度で、原点からの距離は a (= 6378137 m) になるはず。
  // ハードコードした参考値に依存せず、不変量で検証する。
  const GeoCoordinate kG{0.0, 2.5, 0.0};  // 経度は適当な値
  const EcefCoordinate kP = GeoToEcef(kG);
  const double kDist = std::sqrt((kP.X() * kP.X()) + (kP.Y() * kP.Y()) + (kP.Z() * kP.Z()));
  REQUIRE(kDist == Catch::Approx(6378137.0));
  REQUIRE(kP.Z() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("変換ラウンドトリップ (deg <-> rad)", "[coord_conv]") {
  const MapCoordinate kOriginal{35.6812, 139.7671, 40.0};  // 東京駅近辺
  const MapCoordinate kRound = ToMap(ToGeo(kOriginal));
  REQUIRE(kRound.LatitudeDeg() == Catch::Approx(kOriginal.LatitudeDeg()));
  REQUIRE(kRound.LongitudeDeg() == Catch::Approx(kOriginal.LongitudeDeg()));
  REQUIRE(kRound.AltitudeM() == Catch::Approx(kOriginal.AltitudeM()));
}

TEST_CASE("EcefToGeo: 赤道・本初子午線", "[coord_conv][ecef]") {
  // (a, 0, 0) -> (φ=0, λ=0, h=0)
  const GeoCoordinate kG = EcefToGeo(EcefCoordinate{6378137.0, 0.0, 0.0});
  REQUIRE(kG.Latitude() == Catch::Approx(0.0).margin(1e-12));
  REQUIRE(kG.Longitude() == Catch::Approx(0.0).margin(1e-12));
  REQUIRE(kG.Altitude() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("EcefToGeo: 赤道・東経90度", "[coord_conv][ecef]") {
  // (0, a, 0) -> (φ=0, λ=π/2, h=0)
  const GeoCoordinate kG = EcefToGeo(EcefCoordinate{0.0, 6378137.0, 0.0});
  REQUIRE(kG.Latitude() == Catch::Approx(0.0).margin(1e-12));
  REQUIRE(kG.Longitude() == Catch::Approx(1.5707963267948966));
  REQUIRE(kG.Altitude() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("EcefToGeo: 北極 (極近傍ガード経路)", "[coord_conv][ecef]") {
  // (0, 0, b) -> (φ=π/2, λ=任意 (本実装では 0), h=0)
  // b = a(1-f) ≒ 6356752.3142
  const GeoCoordinate kG = EcefToGeo(EcefCoordinate{0.0, 0.0, 6356752.314245179});
  REQUIRE(kG.Latitude() == Catch::Approx(1.5707963267948966));
  REQUIRE(kG.Altitude() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("EcefToGeo: 南極上空 1000m", "[coord_conv][ecef]") {
  // 南極の真上、楕円体高 1000m。Z = -(b + 1000)、X=Y=0 の自転軸上ケース。
  const GeoCoordinate kG = EcefToGeo(EcefCoordinate{0.0, 0.0, -(6356752.314245179 + 1000.0)});
  REQUIRE(kG.Latitude() == Catch::Approx(-1.5707963267948966));
  REQUIRE(kG.Altitude() == Catch::Approx(1000.0).margin(1e-6));
}

TEST_CASE("ecef <-> geo ラウンドトリップ (各種地点)", "[coord_conv][ecef]") {
  // 様々な (緯度, 経度, 高度) を GeoToEcef -> EcefToGeo して元に戻ることを
  // 確認する。Bowring 法は地表近傍で機械精度に収束するので、margin/epsilon
  // は厳しく取ってよい。
  struct Case {
    double lat_deg_;
    double lon_deg_;
    double alt_m_;
  };
  const Case kCases[] = {
      {35.6812, 139.7671, 40.0},   // 東京
      {-33.8688, 151.2093, 58.0},  // シドニー
      {64.1466, -21.9426, 0.0},    // レイキャビク
      {-1.2921, 36.8219, 1795.0},  // ナイロビ (標高あり)
      {45.0, 0.0, 10000.0},        // 北緯45° 高高度
      {0.0, 180.0, 0.0},           // 日付変更線
  };

  for (const auto& c : kCases) {
    const GeoCoordinate kOriginal = ToGeo(MapCoordinate{c.lat_deg_, c.lon_deg_, c.alt_m_});
    const GeoCoordinate kRound = EcefToGeo(GeoToEcef(kOriginal));
    INFO("case lat_deg=" << c.lat_deg_ << " lon_deg=" << c.lon_deg_ << " alt_m=" << c.alt_m_);
    REQUIRE(kRound.Latitude() == Catch::Approx(kOriginal.Latitude()).margin(1e-10));
    REQUIRE(kRound.Longitude() == Catch::Approx(kOriginal.Longitude()).margin(1e-10));
    REQUIRE(kRound.Altitude() == Catch::Approx(kOriginal.Altitude()).margin(1e-6));
  }
}
