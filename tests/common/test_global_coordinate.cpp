#include <catch2/catch_amalgamated.hpp>

#include <type_traits>

#include "common/global_coordinate.h"
#include "common/vector3d.h"

using myapp::common::GlobalCoordinate;
using myapp::common::Vector3D;

TEST_CASE("GlobalCoordinate (ECEF): コンストラクタとアクセサ", "[global][ecef]") {
  const GlobalCoordinate kP{1000.0, -2000.0, 3000.0};
  REQUIRE(kP.X() == Catch::Approx(1000.0));
  REQUIRE(kP.Y() == Catch::Approx(-2000.0));
  REQUIRE(kP.Z() == Catch::Approx(3000.0));
}

TEST_CASE("GlobalCoordinate (ECEF): 位置 - 位置 は変位ベクトル", "[global][ecef]") {
  const GlobalCoordinate kA{10.0, 20.0, 30.0};
  const GlobalCoordinate kB{1.0, 2.0, 3.0};
  const Vector3D kD = kA - kB;
  REQUIRE(kD.x_ == Catch::Approx(9.0));
  REQUIRE(kD.y_ == Catch::Approx(18.0));
  REQUIRE(kD.z_ == Catch::Approx(27.0));
}

TEST_CASE("GlobalCoordinate (ECEF): 位置 + 変位 は新しい位置", "[global][ecef]") {
  const GlobalCoordinate kOrigin{100.0, 200.0, 300.0};
  const Vector3D kDisp{1.0, -2.0, 3.0};
  const GlobalCoordinate kMoved = kOrigin + kDisp;
  REQUIRE(kMoved.X() == Catch::Approx(101.0));
  REQUIRE(kMoved.Y() == Catch::Approx(198.0));
  REQUIRE(kMoved.Z() == Catch::Approx(303.0));
}

TEST_CASE("GlobalCoordinate (ECEF): 位置 - 変位 は新しい位置", "[global][ecef]") {
  const GlobalCoordinate kOrigin{100.0, 200.0, 300.0};
  const Vector3D kDisp{1.0, -2.0, 3.0};
  const GlobalCoordinate kMoved = kOrigin - kDisp;
  REQUIRE(kMoved.X() == Catch::Approx(99.0));
  REQUIRE(kMoved.Y() == Catch::Approx(202.0));
  REQUIRE(kMoved.Z() == Catch::Approx(297.0));
}

TEST_CASE("GlobalCoordinate (ECEF): 線形補間 a + (b - a) * t", "[global][ecef]") {
  const GlobalCoordinate kA{0.0, 0.0, 0.0};
  const GlobalCoordinate kB{10.0, 20.0, 30.0};
  const GlobalCoordinate kMid = kA + (kB - kA) * 0.5;
  REQUIRE(kMid.X() == Catch::Approx(5.0));
  REQUIRE(kMid.Y() == Catch::Approx(10.0));
  REQUIRE(kMid.Z() == Catch::Approx(15.0));
}

// 静的アサーション: 型安全の確認 (コンパイル時に検証される設計上の不変条件)。
// Vector3D <-> GlobalCoordinate の暗黙変換は意図的に許可しない。
static_assert(!std::is_convertible_v<Vector3D, GlobalCoordinate>,
              "Vector3D から GlobalCoordinate への暗黙変換は禁止");
static_assert(!std::is_convertible_v<GlobalCoordinate, Vector3D>,
              "GlobalCoordinate から Vector3D への暗黙変換は禁止");
