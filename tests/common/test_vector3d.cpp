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

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
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

TEST_CASE("Vector3D: 単項マイナス", "[vector3d]") {
  const Vector3D kA{1.0, -2.0, 3.0};
  const Vector3D kR = -kA;
  REQUIRE(kR.x_ == Catch::Approx(-1.0));
  REQUIRE(kR.y_ == Catch::Approx(2.0));
  REQUIRE(kR.z_ == Catch::Approx(-3.0));
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_CASE("Vector3D: 複合代入", "[vector3d]") {
  SECTION("+=") {
    Vector3D a{1.0, 2.0, 3.0};
    a += Vector3D{4.0, 5.0, 6.0};
    REQUIRE(a == Vector3D{5.0, 7.0, 9.0});
  }
  SECTION("-=") {
    Vector3D a{4.0, 5.0, 6.0};
    a -= Vector3D{1.0, 2.0, 3.0};
    REQUIRE(a == Vector3D{3.0, 3.0, 3.0});
  }
  SECTION("*=") {
    Vector3D a{1.0, -2.0, 3.0};
    a *= 2.0;
    REQUIRE(a == Vector3D{2.0, -4.0, 6.0});
  }
  SECTION("/=") {
    Vector3D a{2.0, -4.0, 6.0};
    a /= 2.0;
    REQUIRE(a == Vector3D{1.0, -2.0, 3.0});
  }
}

TEST_CASE("Vector3D: 等値比較", "[vector3d]") {
  REQUIRE(Vector3D{1.0, 2.0, 3.0} == Vector3D{1.0, 2.0, 3.0});
  REQUIRE(Vector3D{1.0, 2.0, 3.0} != Vector3D{1.0, 2.0, 4.0});
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_CASE("Vector3D: 外積", "[vector3d]") {
  const Vector3D kX{1.0, 0.0, 0.0};
  const Vector3D kY{0.0, 1.0, 0.0};
  const Vector3D kZ = kX.Cross(kY);
  REQUIRE(kZ.x_ == Catch::Approx(0.0));
  REQUIRE(kZ.y_ == Catch::Approx(0.0));
  REQUIRE(kZ.z_ == Catch::Approx(1.0));

  SECTION("反対称性: b × a == -(a × b)") {
    const Vector3D kA{1.0, 2.0, 3.0};
    const Vector3D kB{-4.0, 5.0, 6.0};
    const Vector3D kAb = kA.Cross(kB);
    const Vector3D kBa = kB.Cross(kA);
    REQUIRE(kBa.x_ == Catch::Approx(-kAb.x_));
    REQUIRE(kBa.y_ == Catch::Approx(-kAb.y_));
    REQUIRE(kBa.z_ == Catch::Approx(-kAb.z_));
  }
}

TEST_CASE("Vector3D: ノルム", "[vector3d]") {
  const Vector3D kA{3.0, 4.0, 0.0};
  REQUIRE(kA.SquaredNorm() == Catch::Approx(25.0));
  REQUIRE(kA.Norm() == Catch::Approx(5.0));
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_CASE("Vector3D: 正規化", "[vector3d]") {
  SECTION("通常ベクトル") {
    const Vector3D kA{3.0, 4.0, 0.0};
    const Vector3D kN = kA.Normalized();
    REQUIRE(kN.x_ == Catch::Approx(0.6));
    REQUIRE(kN.y_ == Catch::Approx(0.8));
    REQUIRE(kN.z_ == Catch::Approx(0.0));
    REQUIRE(kN.Norm() == Catch::Approx(1.0));
  }
  SECTION("ゼロベクトルは (0,0,0) を返す (NaN を返さない)") {
    const Vector3D kZero{0.0, 0.0, 0.0};
    const Vector3D kN = kZero.Normalized();
    REQUIRE(kN == Vector3D{0.0, 0.0, 0.0});
  }
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
