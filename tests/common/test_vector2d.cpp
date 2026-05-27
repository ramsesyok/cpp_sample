#include <catch2/catch_amalgamated.hpp>

#include "common/vector2d.h"

using myapp::common::Dot;
using myapp::common::Vector2D;

TEST_CASE("Vector2D: 加算", "[vector2d]") {
  const Vector2D kA{1.0, 2.0};
  const Vector2D kB{4.0, 5.0};
  const Vector2D kC = kA + kB;
  REQUIRE(kC.x_ == Catch::Approx(5.0));
  REQUIRE(kC.y_ == Catch::Approx(7.0));
}

TEST_CASE("Vector2D: 減算", "[vector2d]") {
  const Vector2D kA{4.0, 5.0};
  const Vector2D kB{1.0, 2.0};
  const Vector2D kC = kA - kB;
  REQUIRE(kC.x_ == Catch::Approx(3.0));
  REQUIRE(kC.y_ == Catch::Approx(3.0));
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_CASE("Vector2D: スカラー乗算", "[vector2d]") {
  const Vector2D kA{1.0, -2.0};
  const Vector2D kR = kA * 2.0;
  REQUIRE(kR.x_ == Catch::Approx(2.0));
  REQUIRE(kR.y_ == Catch::Approx(-4.0));

  SECTION("左辺スカラーも可") {
    const Vector2D kL = 2.0 * kA;
    REQUIRE(kL.x_ == Catch::Approx(2.0));
    REQUIRE(kL.y_ == Catch::Approx(-4.0));
  }
}

TEST_CASE("Vector2D: スカラー除算", "[vector2d]") {
  const Vector2D kA{2.0, -4.0};
  const Vector2D kR = kA / 2.0;
  REQUIRE(kR.x_ == Catch::Approx(1.0));
  REQUIRE(kR.y_ == Catch::Approx(-2.0));
}

TEST_CASE("Vector2D: 単項マイナス", "[vector2d]") {
  const Vector2D kA{1.0, -2.0};
  const Vector2D kR = -kA;
  REQUIRE(kR.x_ == Catch::Approx(-1.0));
  REQUIRE(kR.y_ == Catch::Approx(2.0));
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_CASE("Vector2D: 複合代入", "[vector2d]") {
  SECTION("+=") {
    Vector2D a{1.0, 2.0};
    a += Vector2D{4.0, 5.0};
    REQUIRE(a == Vector2D{5.0, 7.0});
  }
  SECTION("-=") {
    Vector2D a{4.0, 5.0};
    a -= Vector2D{1.0, 2.0};
    REQUIRE(a == Vector2D{3.0, 3.0});
  }
  SECTION("*=") {
    Vector2D a{1.0, -2.0};
    a *= 2.0;
    REQUIRE(a == Vector2D{2.0, -4.0});
  }
  SECTION("/=") {
    Vector2D a{2.0, -4.0};
    a /= 2.0;
    REQUIRE(a == Vector2D{1.0, -2.0});
  }
}

TEST_CASE("Vector2D: 等値比較", "[vector2d]") {
  REQUIRE(Vector2D{1.0, 2.0} == Vector2D{1.0, 2.0});
  REQUIRE(Vector2D{1.0, 2.0} != Vector2D{1.0, 3.0});
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_CASE("Vector2D: 外積 (スカラー)", "[vector2d]") {
  const Vector2D kX{1.0, 0.0};
  const Vector2D kY{0.0, 1.0};
  REQUIRE(kX.Cross(kY) == Catch::Approx(1.0));

  SECTION("反対称性: b × a == -(a × b)") {
    const Vector2D kA{1.0, 2.0};
    const Vector2D kB{-4.0, 5.0};
    REQUIRE(kB.Cross(kA) == Catch::Approx(-kA.Cross(kB)));
  }

  SECTION("平行ベクトルは 0") {
    const Vector2D kA{2.0, 3.0};
    const Vector2D kB{4.0, 6.0};
    REQUIRE(kA.Cross(kB) == Catch::Approx(0.0));
  }
}

TEST_CASE("Vector2D: ノルム", "[vector2d]") {
  const Vector2D kA{3.0, 4.0};
  REQUIRE(kA.SquaredNorm() == Catch::Approx(25.0));
  REQUIRE(kA.Norm() == Catch::Approx(5.0));
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_CASE("Vector2D: 正規化", "[vector2d]") {
  SECTION("通常ベクトル") {
    const Vector2D kA{3.0, 4.0};
    const Vector2D kN = kA.Normalized();
    REQUIRE(kN.x_ == Catch::Approx(0.6));
    REQUIRE(kN.y_ == Catch::Approx(0.8));
    REQUIRE(kN.Norm() == Catch::Approx(1.0));
  }
  SECTION("ゼロベクトルは (0,0) を返す (NaN を返さない)") {
    const Vector2D kZero{0.0, 0.0};
    const Vector2D kN = kZero.Normalized();
    REQUIRE(kN == Vector2D{0.0, 0.0});
  }
}

TEST_CASE("Vector2D: 内積", "[vector2d]") {
  SECTION("一般ケース") {
    const Vector2D kA{1.0, 2.0};
    const Vector2D kB{4.0, -5.0};
    REQUIRE(kA.Dot(kB) == Catch::Approx((1.0 * 4.0) + (2.0 * -5.0)));
  }
  SECTION("直交ベクトルは 0") {
    const Vector2D kX{1.0, 0.0};
    const Vector2D kY{0.0, 1.0};
    REQUIRE(kX.Dot(kY) == Catch::Approx(0.0));
  }
  SECTION("自身との内積はノルム二乗") {
    const Vector2D kA{3.0, 4.0};
    REQUIRE(kA.Dot(kA) == Catch::Approx(25.0));
  }
  SECTION("自由関数版 Dot(a, b) はメンバ版と等価") {
    const Vector2D kA{1.0, 2.0};
    const Vector2D kB{4.0, -5.0};
    REQUIRE(Dot(kA, kB) == Catch::Approx(kA.Dot(kB)));
  }
}
