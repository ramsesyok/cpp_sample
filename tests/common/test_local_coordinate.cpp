#include <catch2/catch_amalgamated.hpp>

#include <type_traits>

#include "common/global_coordinate.h"
#include "common/local_coordinate.h"
#include "common/vector3d.h"

using myapp::common::GlobalCoordinate;
using myapp::common::LocalCoordinate;
using myapp::common::Vector3D;

TEST_CASE("LocalCoordinate: コンストラクタとアクセサ", "[local]") {
  const LocalCoordinate kP{10.0, -20.0, 30.0};
  REQUIRE(kP.X() == Catch::Approx(10.0));
  REQUIRE(kP.Y() == Catch::Approx(-20.0));
  REQUIRE(kP.Z() == Catch::Approx(30.0));
}

TEST_CASE("LocalCoordinate: 位置 - 位置 は変位ベクトル", "[local]") {
  const LocalCoordinate kA{10.0, 20.0, 30.0};
  const LocalCoordinate kB{1.0, 2.0, 3.0};
  const Vector3D kD = kA - kB;
  REQUIRE(kD.x_ == Catch::Approx(9.0));
  REQUIRE(kD.y_ == Catch::Approx(18.0));
  REQUIRE(kD.z_ == Catch::Approx(27.0));
}

TEST_CASE("LocalCoordinate: 位置 ± 変位 は新しい位置", "[local]") {
  const LocalCoordinate kOrigin{100.0, 200.0, 300.0};
  const Vector3D kDisp{1.0, -2.0, 3.0};
  const LocalCoordinate kPlus = kOrigin + kDisp;
  REQUIRE(kPlus.X() == Catch::Approx(101.0));
  REQUIRE(kPlus.Y() == Catch::Approx(198.0));
  REQUIRE(kPlus.Z() == Catch::Approx(303.0));

  const LocalCoordinate kMinus = kOrigin - kDisp;
  REQUIRE(kMinus.X() == Catch::Approx(99.0));
  REQUIRE(kMinus.Y() == Catch::Approx(202.0));
  REQUIRE(kMinus.Z() == Catch::Approx(297.0));
}

// 他座標系・Vector3D との取り違えを型で防ぐ。
static_assert(!std::is_convertible_v<Vector3D, LocalCoordinate>,
              "Vector3D から LocalCoordinate への暗黙変換は禁止");
static_assert(!std::is_convertible_v<LocalCoordinate, Vector3D>,
              "LocalCoordinate から Vector3D への暗黙変換は禁止");
static_assert(!std::is_convertible_v<GlobalCoordinate, LocalCoordinate>,
              "GlobalCoordinate から LocalCoordinate への暗黙変換は禁止");
static_assert(!std::is_convertible_v<LocalCoordinate, GlobalCoordinate>,
              "LocalCoordinate から GlobalCoordinate への暗黙変換は禁止");
