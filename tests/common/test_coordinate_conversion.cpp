#include <catch2/catch_amalgamated.hpp>

#include <array>
#include <cmath>

#include "common/attitude.h"
#include "common/body_coordinate.h"
#include "common/constants.h"
#include "common/coordinate_conversion.h"
#include "common/global_coordinate.h"
#include "common/local_coordinate.h"

using myapp::common::Attitude;
using myapp::common::BodyCoordinate;
using myapp::common::BodyToLocal;
using myapp::common::GeoCoordinate;
using myapp::common::GeoToGlobal;
using myapp::common::GeoToMap;
using myapp::common::GlobalCoordinate;
using myapp::common::GlobalToGeo;
using myapp::common::GlobalToLocal;
using myapp::common::kHalfPi;
using myapp::common::LocalCoordinate;
using myapp::common::LocalToBody;
using myapp::common::LocalToGlobal;
using myapp::common::MapCoordinate;
using myapp::common::MapToGeo;

TEST_CASE("GeoToMap: ラジアン -> 度", "[coord_conv]") {
  // pi/4 rad = 45 deg
  const GeoCoordinate kG{0.7853981633974483, 1.5707963267948966, 123.0};
  const MapCoordinate kM = GeoToMap(kG);
  REQUIRE(kM.LatitudeDeg() == Catch::Approx(45.0));
  REQUIRE(kM.LongitudeDeg() == Catch::Approx(90.0));
  REQUIRE(kM.AltitudeM() == Catch::Approx(123.0));
}

TEST_CASE("MapToGeo: 度 -> ラジアン", "[coord_conv]") {
  const MapCoordinate kM{180.0, -90.0, 50.0};
  const GeoCoordinate kG = MapToGeo(kM);
  REQUIRE(kG.Latitude() == Catch::Approx(3.141592653589793));
  REQUIRE(kG.Longitude() == Catch::Approx(-1.5707963267948966));
  REQUIRE(kG.Altitude() == Catch::Approx(50.0));
}

TEST_CASE("GeoToGlobal: 赤道・本初子午線・標高0", "[coord_conv][ecef]") {
  // φ=0, λ=0, h=0 -> (a, 0, 0)
  const GeoCoordinate kG{0.0, 0.0, 0.0};
  const GlobalCoordinate kP = GeoToGlobal(kG);
  REQUIRE(kP.X() == Catch::Approx(6378137.0));
  REQUIRE(kP.Y() == Catch::Approx(0.0).margin(1e-6));
  REQUIRE(kP.Z() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("GeoToGlobal: 赤道・東経90度・標高0", "[coord_conv][ecef]") {
  // φ=0, λ=π/2 -> (0, a, 0)
  const GeoCoordinate kG{0.0, 1.5707963267948966, 0.0};
  const GlobalCoordinate kP = GeoToGlobal(kG);
  REQUIRE(kP.X() == Catch::Approx(0.0).margin(1e-6));
  REQUIRE(kP.Y() == Catch::Approx(6378137.0));
  REQUIRE(kP.Z() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("GeoToGlobal: 北極", "[coord_conv][ecef]") {
  // φ=π/2 -> (0, 0, b)  ここで b = a*(1-f) ≒ 6356752.3142 (WGS84 短半径)
  const GeoCoordinate kG{1.5707963267948966, 0.0, 0.0};
  const GlobalCoordinate kP = GeoToGlobal(kG);
  REQUIRE(kP.X() == Catch::Approx(0.0).margin(1e-6));
  REQUIRE(kP.Y() == Catch::Approx(0.0).margin(1e-6));
  REQUIRE(kP.Z() == Catch::Approx(6356752.314245179));
}

TEST_CASE("GeoToGlobal: 標高が直接 X に加算される (赤道)", "[coord_conv][ecef]") {
  // φ=0, λ=0, h=1000 -> (a+1000, 0, 0)
  const GeoCoordinate kG{0.0, 0.0, 1000.0};
  const GlobalCoordinate kP = GeoToGlobal(kG);
  REQUIRE(kP.X() == Catch::Approx(6378137.0 + 1000.0));
  REQUIRE(kP.Y() == Catch::Approx(0.0).margin(1e-6));
  REQUIRE(kP.Z() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("GeoToGlobal: 原点からの距離が a に近い (赤道)", "[coord_conv][ecef]") {
  // 赤道上の任意の経度で、原点からの距離は a (= 6378137 m) になるはず。
  // ハードコードした参考値に依存せず、不変量で検証する。
  const GeoCoordinate kG{0.0, 2.5, 0.0};  // 経度は適当な値
  const GlobalCoordinate kP = GeoToGlobal(kG);
  const double kDist = std::sqrt((kP.X() * kP.X()) + (kP.Y() * kP.Y()) + (kP.Z() * kP.Z()));
  REQUIRE(kDist == Catch::Approx(6378137.0));
  REQUIRE(kP.Z() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("変換ラウンドトリップ (deg <-> rad)", "[coord_conv]") {
  const MapCoordinate kOriginal{35.6812, 139.7671, 40.0};  // 東京駅近辺
  const MapCoordinate kRound = GeoToMap(MapToGeo(kOriginal));
  REQUIRE(kRound.LatitudeDeg() == Catch::Approx(kOriginal.LatitudeDeg()));
  REQUIRE(kRound.LongitudeDeg() == Catch::Approx(kOriginal.LongitudeDeg()));
  REQUIRE(kRound.AltitudeM() == Catch::Approx(kOriginal.AltitudeM()));
}

TEST_CASE("GlobalToGeo: 赤道・本初子午線", "[coord_conv][ecef]") {
  // (a, 0, 0) -> (φ=0, λ=0, h=0)
  const GeoCoordinate kG = GlobalToGeo(GlobalCoordinate{6378137.0, 0.0, 0.0});
  REQUIRE(kG.Latitude() == Catch::Approx(0.0).margin(1e-12));
  REQUIRE(kG.Longitude() == Catch::Approx(0.0).margin(1e-12));
  REQUIRE(kG.Altitude() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("GlobalToGeo: 赤道・東経90度", "[coord_conv][ecef]") {
  // (0, a, 0) -> (φ=0, λ=π/2, h=0)
  const GeoCoordinate kG = GlobalToGeo(GlobalCoordinate{0.0, 6378137.0, 0.0});
  REQUIRE(kG.Latitude() == Catch::Approx(0.0).margin(1e-12));
  REQUIRE(kG.Longitude() == Catch::Approx(1.5707963267948966));
  REQUIRE(kG.Altitude() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("GlobalToGeo: 北極 (極近傍ガード経路)", "[coord_conv][ecef]") {
  // (0, 0, b) -> (φ=π/2, λ=任意 (本実装では 0), h=0)
  // b = a(1-f) ≒ 6356752.3142
  const GeoCoordinate kG = GlobalToGeo(GlobalCoordinate{0.0, 0.0, 6356752.314245179});
  REQUIRE(kG.Latitude() == Catch::Approx(1.5707963267948966));
  REQUIRE(kG.Altitude() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("GlobalToGeo: 南極上空 1000m", "[coord_conv][ecef]") {
  // 南極の真上、楕円体高 1000m。Z = -(b + 1000)、X=Y=0 の自転軸上ケース。
  const GeoCoordinate kG = GlobalToGeo(GlobalCoordinate{0.0, 0.0, -(6356752.314245179 + 1000.0)});
  REQUIRE(kG.Latitude() == Catch::Approx(-1.5707963267948966));
  REQUIRE(kG.Altitude() == Catch::Approx(1000.0).margin(1e-6));
}

TEST_CASE("GlobalToGeo: 自転軸近傍ガード (p < 1mm)", "[coord_conv][ecef]") {
  // X=Y=0 だが浮動小数演算で生じる程度の微小値が入っているケース。
  // 閾値 (1mm) 未満なら極ケースに落ちて、緯度は ±π/2 / 経度は 0 になる。
  const GeoCoordinate kG = GlobalToGeo(GlobalCoordinate{1e-6, -1e-6, 6356752.314245179});
  REQUIRE(kG.Latitude() == Catch::Approx(kHalfPi));
  REQUIRE(kG.Longitude() == Catch::Approx(0.0));
  REQUIRE(kG.Altitude() == Catch::Approx(0.0).margin(1e-3));
}

TEST_CASE("ecef <-> geo ラウンドトリップ (各種地点)", "[coord_conv][ecef]") {
  // 様々な (緯度, 経度, 高度) を GeoToGlobal -> GlobalToGeo して元に戻ることを
  // 確認する。Bowring 法は地表近傍で機械精度に収束するので、margin/epsilon
  // は厳しく取ってよい。
  struct Case {
    double lat_deg_;
    double lon_deg_;
    double alt_m_;
  };
  const std::array<Case, 6> kCases = {{
      {35.6812, 139.7671, 40.0},   // 東京
      {-33.8688, 151.2093, 58.0},  // シドニー
      {64.1466, -21.9426, 0.0},    // レイキャビク
      {-1.2921, 36.8219, 1795.0},  // ナイロビ (標高あり)
      {45.0, 0.0, 10000.0},        // 北緯45° 高高度
      {0.0, 180.0, 0.0},           // 日付変更線
  }};

  for (const auto& c : kCases) {
    const GeoCoordinate kOriginal = MapToGeo(MapCoordinate{c.lat_deg_, c.lon_deg_, c.alt_m_});
    const GeoCoordinate kRound = GlobalToGeo(GeoToGlobal(kOriginal));
    INFO("case lat_deg=" << c.lat_deg_ << " lon_deg=" << c.lon_deg_ << " alt_m=" << c.alt_m_);
    REQUIRE(kRound.Latitude() == Catch::Approx(kOriginal.Latitude()).margin(1e-10));
    REQUIRE(kRound.Longitude() == Catch::Approx(kOriginal.Longitude()).margin(1e-10));
    REQUIRE(kRound.Altitude() == Catch::Approx(kOriginal.Altitude()).margin(1e-6));
  }
}

TEST_CASE("LocalToGlobal / GlobalToLocal: 原点ラウンドトリップ", "[coord_conv][ned]") {
  // 参照点の真上に立つ NED(0,0,0) 位置は、ECEF に変換しても参照点 ECEF と一致する。
  const GeoCoordinate kRef = MapToGeo(MapCoordinate{35.6812, 139.7671, 40.0});
  const GlobalCoordinate kRefEcef = GeoToGlobal(kRef);
  const GlobalCoordinate kP = LocalToGlobal(LocalCoordinate{0.0, 0.0, 0.0}, kRef);
  REQUIRE(kP.X() == Catch::Approx(kRefEcef.X()));
  REQUIRE(kP.Y() == Catch::Approx(kRefEcef.Y()));
  REQUIRE(kP.Z() == Catch::Approx(kRefEcef.Z()));

  const LocalCoordinate kBack = GlobalToLocal(kP, kRef);
  REQUIRE(kBack.X() == Catch::Approx(0.0).margin(1e-6));
  REQUIRE(kBack.Y() == Catch::Approx(0.0).margin(1e-6));
  REQUIRE(kBack.Z() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE("LocalToGlobal / GlobalToLocal: 一般ラウンドトリップ", "[coord_conv][ned]") {
  const GeoCoordinate kRef = MapToGeo(MapCoordinate{35.6812, 139.7671, 40.0});
  const LocalCoordinate kOriginal{100.0, -50.0, 25.0};  // 北100m, 東-50m, 下25m
  const GlobalCoordinate kP = LocalToGlobal(kOriginal, kRef);
  const LocalCoordinate kBack = GlobalToLocal(kP, kRef);
  REQUIRE(kBack.X() == Catch::Approx(kOriginal.X()).margin(1e-6));
  REQUIRE(kBack.Y() == Catch::Approx(kOriginal.Y()).margin(1e-6));
  REQUIRE(kBack.Z() == Catch::Approx(kOriginal.Z()).margin(1e-6));
}

TEST_CASE("LocalToGlobal: 赤道・本初子午線で N/E/D が ECEF 軸に整列", "[coord_conv][ned]") {
  // 赤道・本初子午線 (φ=0, λ=0) では、
  //   N 軸 = +Z ECEF, E 軸 = +Y ECEF, D 軸 = -X ECEF
  // となる (NedToGlobalRotation の代数から確認可能)。
  const GeoCoordinate kRef{0.0, 0.0, 0.0};
  const GlobalCoordinate kRefEcef = GeoToGlobal(kRef);

  // 北 1m
  const GlobalCoordinate kN = LocalToGlobal(LocalCoordinate{1.0, 0.0, 0.0}, kRef);
  REQUIRE((kN.Z() - kRefEcef.Z()) == Catch::Approx(1.0).margin(1e-9));
  REQUIRE((kN.X() - kRefEcef.X()) == Catch::Approx(0.0).margin(1e-9));
  REQUIRE((kN.Y() - kRefEcef.Y()) == Catch::Approx(0.0).margin(1e-9));

  // 東 1m
  const GlobalCoordinate kE = LocalToGlobal(LocalCoordinate{0.0, 1.0, 0.0}, kRef);
  REQUIRE((kE.Y() - kRefEcef.Y()) == Catch::Approx(1.0).margin(1e-9));

  // 下 1m  (= ECEF -X 方向)
  const GlobalCoordinate kD = LocalToGlobal(LocalCoordinate{0.0, 0.0, 1.0}, kRef);
  REQUIRE((kD.X() - kRefEcef.X()) == Catch::Approx(-1.0).margin(1e-9));
}

TEST_CASE("BodyToLocal / LocalToBody: ゼロ姿勢は恒等", "[coord_conv][body]") {
  const Attitude kZero{0.0, 0.0, 0.0};
  const BodyCoordinate kB{1.0, 2.0, 3.0};
  const LocalCoordinate kL = BodyToLocal(kB, kZero);
  REQUIRE(kL.X() == Catch::Approx(1.0));
  REQUIRE(kL.Y() == Catch::Approx(2.0));
  REQUIRE(kL.Z() == Catch::Approx(3.0));
}

TEST_CASE("BodyToLocal: yaw 90度は body +X を NED +E に写す", "[coord_conv][body]") {
  // yaw=+π/2 で機体 +X (前方) は NED の +E (東) を向く。
  const Attitude kYaw{0.0, 0.0, kHalfPi};
  const LocalCoordinate kL = BodyToLocal(BodyCoordinate{1.0, 0.0, 0.0}, kYaw);
  REQUIRE(kL.X() == Catch::Approx(0.0).margin(1e-12));
  REQUIRE(kL.Y() == Catch::Approx(1.0).margin(1e-12));
  REQUIRE(kL.Z() == Catch::Approx(0.0).margin(1e-12));
}

TEST_CASE("BodyToLocal / LocalToBody: ラウンドトリップ", "[coord_conv][body]") {
  const Attitude kAtt{0.3, -0.7, 1.2};
  const BodyCoordinate kOriginal{1.5, -2.5, 0.7};
  const LocalCoordinate kL = BodyToLocal(kOriginal, kAtt);
  const BodyCoordinate kBack = LocalToBody(kL, kAtt);
  REQUIRE(kBack.X() == Catch::Approx(kOriginal.X()).margin(1e-12));
  REQUIRE(kBack.Y() == Catch::Approx(kOriginal.Y()).margin(1e-12));
  REQUIRE(kBack.Z() == Catch::Approx(kOriginal.Z()).margin(1e-12));
}
