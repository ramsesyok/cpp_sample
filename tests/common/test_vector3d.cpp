#include <catch2/catch_amalgamated.hpp>

#include "common/vector3d.h"

using myapp::common::Vector3D;

TEST_CASE("Vector3D: 加算", "[vector3d]") {
  const Vector3D kA{1.0, 2.0, 3.0};
  const Vector3D kB{4.0, 5.0, 6.0};
  const Vector3D kC = kA + kB;
  REQUIRE(kC.x_ == Catch::Approx(5.0));
  REQUIRE(kC.y_ == Catch::Approx(7.0));
  REQUIRE(kC.z_ == Catch::Approx(9.0));
}

TEST_CASE("Vector3D: 減算", "[vector3d]") {
  const Vector3D kA{4.0, 5.0, 6.0};
  const Vector3D kB{1.0, 2.0, 3.0};
  const Vector3D kC = kA - kB;
  REQUIRE(kC.x_ == Catch::Approx(3.0));
  REQUIRE(kC.y_ == Catch::Approx(3.0));
  REQUIRE(kC.z_ == Catch::Approx(3.0));
}

TEST_CASE("Vector3D: スカラー乗算", "[vector3d]") {
  const Vector3D kA{1.0, -2.0, 3.0};
  const Vector3D kR = kA * 2.0;
  REQUIRE(kR.x_ == Catch::Approx(2.0));
  REQUIRE(kR.y_ == Catch::Approx(-4.0));
  REQUIRE(kR.z_ == Catch::Approx(6.0));

  SECTION("左辺スカラーも可") {
    const Vector3D kL = 2.0 * kA;
    REQUIRE(kL.x_ == Catch::Approx(2.0));
    REQUIRE(kL.y_ == Catch::Approx(-4.0));
    REQUIRE(kL.z_ == Catch::Approx(6.0));
  }
}

TEST_CASE("Vector3D: スカラー除算", "[vector3d]") {
  const Vector3D kA{2.0, -4.0, 6.0};
  const Vector3D kR = kA / 2.0;
  REQUIRE(kR.x_ == Catch::Approx(1.0));
  REQUIRE(kR.y_ == Catch::Approx(-2.0));
  REQUIRE(kR.z_ == Catch::Approx(3.0));
}

TEST_CASE("Vector3D: 内積", "[vector3d]") {
  SECTION("一般ケース") {
    const Vector3D kA{1.0, 2.0, 3.0};
    const Vector3D kB{4.0, -5.0, 6.0};
    REQUIRE(kA.Dot(kB) == Catch::Approx((1.0 * 4.0) + (2.0 * -5.0) + (3.0 * 6.0)));
  }
  SECTION("直交ベクトルは 0") {
    const Vector3D kX{1.0, 0.0, 0.0};
    const Vector3D kY{0.0, 1.0, 0.0};
    REQUIRE(kX.Dot(kY) == Catch::Approx(0.0));
  }
  SECTION("自身との内積はノルム二乗") {
    const Vector3D kA{3.0, 4.0, 0.0};
    REQUIRE(kA.Dot(kA) == Catch::Approx(25.0));
  }
}
