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

