#include <catch2/catch_amalgamated.hpp>

#include <type_traits>

#include "common/ecef_coordinate.h"
#include "common/vector3d.h"

using myapp::common::EcefCoordinate;
using myapp::common::Vector3D;

TEST_CASE("EcefCoordinate: コンストラクタとアクセサ", "[ecef]") {
  const EcefCoordinate kP{1000.0, -2000.0, 3000.0};
  REQUIRE(kP.X() == Catch::Approx(1000.0));
  REQUIRE(kP.Y() == Catch::Approx(-2000.0));
  REQUIRE(kP.Z() == Catch::Approx(3000.0));
}

TEST_CASE("EcefCoordinate: 位置 - 位置 は変位ベクトル", "[ecef]") {
  const EcefCoordinate kA{10.0, 20.0, 30.0};
  const EcefCoordinate kB{1.0, 2.0, 3.0};
  const Vector3D kD = kA - kB;
  REQUIRE(kD.x_ == Catch::Approx(9.0));
  REQUIRE(kD.y_ == Catch::Approx(18.0));
  REQUIRE(kD.z_ == Catch::Approx(27.0));
}

TEST_CASE("EcefCoordinate: 位置 + 変位 は新しい位置", "[ecef]") {
  const EcefCoordinate kOrigin{100.0, 200.0, 300.0};
  const Vector3D kDisp{1.0, -2.0, 3.0};
  const EcefCoordinate kMoved = kOrigin + kDisp;
  REQUIRE(kMoved.X() == Catch::Approx(101.0));
  REQUIRE(kMoved.Y() == Catch::Approx(198.0));
  REQUIRE(kMoved.Z() == Catch::Approx(303.0));
}

TEST_CASE("EcefCoordinate: スカラー倍は各成分を s 倍する", "[ecef]") {
  const EcefCoordinate kP{2.0, -4.0, 6.0};
  const EcefCoordinate kQ = kP * 0.5;
  REQUIRE(kQ.X() == Catch::Approx(1.0));
  REQUIRE(kQ.Y() == Catch::Approx(-2.0));
  REQUIRE(kQ.Z() == Catch::Approx(3.0));
}

TEST_CASE("EcefCoordinate: スカラー除算は各成分を s で割る", "[ecef]") {
  const EcefCoordinate kP{2.0, -4.0, 6.0};
  const EcefCoordinate kQ = kP / 2.0;
  REQUIRE(kQ.X() == Catch::Approx(1.0));
  REQUIRE(kQ.Y() == Catch::Approx(-2.0));
  REQUIRE(kQ.Z() == Catch::Approx(3.0));
}

TEST_CASE("EcefCoordinate: 線形結合で位置補間が組める", "[ecef]") {
  // a * (1 - t) + b * t の形で中点を求める。
  const EcefCoordinate kA{0.0, 0.0, 0.0};
  const EcefCoordinate kB{10.0, 20.0, 30.0};
  const EcefCoordinate kMid = kA * 0.5 + (kB - kA) * 0.5;
  REQUIRE(kMid.X() == Catch::Approx(5.0));
  REQUIRE(kMid.Y() == Catch::Approx(10.0));
  REQUIRE(kMid.Z() == Catch::Approx(15.0));
}

// 静的アサーション: 型安全の確認 (コンパイル時に検証される設計上の不変条件)。
// Vector3D <-> EcefCoordinate の暗黙変換は意図的に許可しない。
static_assert(!std::is_convertible_v<Vector3D, EcefCoordinate>,
              "Vector3D から EcefCoordinate への暗黙変換は禁止");
static_assert(!std::is_convertible_v<EcefCoordinate, Vector3D>,
              "EcefCoordinate から Vector3D への暗黙変換は禁止");
