#include <catch2/catch_amalgamated.hpp>

#include <type_traits>

#include "common/body_coordinate.h"
#include "common/local_coordinate.h"
#include "common/vector3d.h"

using myapp::common::BodyCoordinate;
using myapp::common::LocalCoordinate;
using myapp::common::Vector3D;

TEST_CASE("BodyCoordinate: コンストラクタとアクセサ", "[body]") {
  const BodyCoordinate kP{1.0, 2.0, 3.0};
  REQUIRE(kP.X() == Catch::Approx(1.0));
  REQUIRE(kP.Y() == Catch::Approx(2.0));
  REQUIRE(kP.Z() == Catch::Approx(3.0));
}

TEST_CASE("BodyCoordinate: 位置 - 位置 は変位ベクトル", "[body]") {
  const BodyCoordinate kA{10.0, 20.0, 30.0};
  const BodyCoordinate kB{1.0, 2.0, 3.0};
  const Vector3D kD = kA - kB;
  REQUIRE(kD.x_ == Catch::Approx(9.0));
  REQUIRE(kD.y_ == Catch::Approx(18.0));
  REQUIRE(kD.z_ == Catch::Approx(27.0));
}

TEST_CASE("BodyCoordinate: 位置 ± 変位 は新しい位置", "[body]") {
  const BodyCoordinate kOrigin{1.0, 2.0, 3.0};
  const Vector3D kDisp{0.5, -0.5, 1.0};
  const BodyCoordinate kPlus = kOrigin + kDisp;
  REQUIRE(kPlus.X() == Catch::Approx(1.5));
  REQUIRE(kPlus.Y() == Catch::Approx(1.5));
  REQUIRE(kPlus.Z() == Catch::Approx(4.0));

  const BodyCoordinate kMinus = kOrigin - kDisp;
  REQUIRE(kMinus.X() == Catch::Approx(0.5));
  REQUIRE(kMinus.Y() == Catch::Approx(2.5));
  REQUIRE(kMinus.Z() == Catch::Approx(2.0));
}

static_assert(!std::is_convertible_v<Vector3D, BodyCoordinate>,
              "Vector3D から BodyCoordinate への暗黙変換は禁止");
static_assert(!std::is_convertible_v<BodyCoordinate, Vector3D>,
              "BodyCoordinate から Vector3D への暗黙変換は禁止");
static_assert(!std::is_convertible_v<LocalCoordinate, BodyCoordinate>,
              "LocalCoordinate から BodyCoordinate への暗黙変換は禁止");
static_assert(!std::is_convertible_v<BodyCoordinate, LocalCoordinate>,
              "BodyCoordinate から LocalCoordinate への暗黙変換は禁止");
