#include <catch2/catch_amalgamated.hpp>

#include "common/attitude.h"

using myapp::common::Attitude;

TEST_CASE("Attitude: 加算", "[attitude]") {
  const Attitude kA{1.0, 2.0, 3.0};
  const Attitude kB{4.0, 5.0, 6.0};
  const Attitude kC = kA + kB;
  REQUIRE(kC.phi_ == Catch::Approx(5.0));
  REQUIRE(kC.theta_ == Catch::Approx(7.0));
  REQUIRE(kC.psi_ == Catch::Approx(9.0));
}

TEST_CASE("Attitude: 減算", "[attitude]") {
  const Attitude kA{4.0, 5.0, 6.0};
  const Attitude kB{1.0, 2.0, 3.0};
  const Attitude kC = kA - kB;
  REQUIRE(kC.phi_ == Catch::Approx(3.0));
  REQUIRE(kC.theta_ == Catch::Approx(3.0));
  REQUIRE(kC.psi_ == Catch::Approx(3.0));
}

TEST_CASE("Attitude: スカラー乗算", "[attitude]") {
  const Attitude kA{1.0, -2.0, 3.0};
  const Attitude kR = kA * 2.0;
  REQUIRE(kR.phi_ == Catch::Approx(2.0));
  REQUIRE(kR.theta_ == Catch::Approx(-4.0));
  REQUIRE(kR.psi_ == Catch::Approx(6.0));

  SECTION("左辺スカラーも可") {
    const Attitude kL = 2.0 * kA;
    REQUIRE(kL.phi_ == Catch::Approx(2.0));
    REQUIRE(kL.theta_ == Catch::Approx(-4.0));
    REQUIRE(kL.psi_ == Catch::Approx(6.0));
  }
}

TEST_CASE("Attitude: スカラー除算", "[attitude]") {
  const Attitude kA{2.0, -4.0, 6.0};
  const Attitude kR = kA / 2.0;
  REQUIRE(kR.phi_ == Catch::Approx(1.0));
  REQUIRE(kR.theta_ == Catch::Approx(-2.0));
  REQUIRE(kR.psi_ == Catch::Approx(3.0));
}

TEST_CASE("Attitude: 内積", "[attitude]") {
  SECTION("一般ケース") {
    const Attitude kA{1.0, 2.0, 3.0};
    const Attitude kB{4.0, -5.0, 6.0};
    REQUIRE(kA.Dot(kB) == Catch::Approx((1.0 * 4.0) + (2.0 * -5.0) + (3.0 * 6.0)));
  }
  SECTION("直交ベクトルは 0") {
    const Attitude kX{1.0, 0.0, 0.0};
    const Attitude kY{0.0, 1.0, 0.0};
    REQUIRE(kX.Dot(kY) == Catch::Approx(0.0));
  }
  SECTION("自身との内積はノルム二乗") {
    const Attitude kA{3.0, 4.0, 0.0};
    REQUIRE(kA.Dot(kA) == Catch::Approx(25.0));
  }
}
