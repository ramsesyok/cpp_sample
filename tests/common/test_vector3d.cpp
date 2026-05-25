#include <catch2/catch_amalgamated.hpp>

#include "common/vector3d.h"

using myapp::common::Vector3D;

TEST_CASE("Vector3D: 加算", "[vector3d]") {
    const Vector3D a{1.0, 2.0, 3.0};
    const Vector3D b{4.0, 5.0, 6.0};
    const Vector3D c = a + b;
    REQUIRE(c.x == Catch::Approx(5.0));
    REQUIRE(c.y == Catch::Approx(7.0));
    REQUIRE(c.z == Catch::Approx(9.0));
}

TEST_CASE("Vector3D: 減算", "[vector3d]") {
    const Vector3D a{4.0, 5.0, 6.0};
    const Vector3D b{1.0, 2.0, 3.0};
    const Vector3D c = a - b;
    REQUIRE(c.x == Catch::Approx(3.0));
    REQUIRE(c.y == Catch::Approx(3.0));
    REQUIRE(c.z == Catch::Approx(3.0));
}

TEST_CASE("Vector3D: スカラー乗算", "[vector3d]") {
    const Vector3D a{1.0, -2.0, 3.0};
    const Vector3D r = a * 2.0;
    REQUIRE(r.x == Catch::Approx(2.0));
    REQUIRE(r.y == Catch::Approx(-4.0));
    REQUIRE(r.z == Catch::Approx(6.0));

    SECTION("左辺スカラーも可") {
        const Vector3D l = 2.0 * a;
        REQUIRE(l.x == Catch::Approx(2.0));
        REQUIRE(l.y == Catch::Approx(-4.0));
        REQUIRE(l.z == Catch::Approx(6.0));
    }
}

TEST_CASE("Vector3D: スカラー除算", "[vector3d]") {
    const Vector3D a{2.0, -4.0, 6.0};
    const Vector3D r = a / 2.0;
    REQUIRE(r.x == Catch::Approx(1.0));
    REQUIRE(r.y == Catch::Approx(-2.0));
    REQUIRE(r.z == Catch::Approx(3.0));
}

TEST_CASE("Vector3D: 内積", "[vector3d]") {
    SECTION("一般ケース") {
        const Vector3D a{1.0, 2.0, 3.0};
        const Vector3D b{4.0, -5.0, 6.0};
        REQUIRE(a.dot(b) == Catch::Approx(1.0 * 4.0 + 2.0 * -5.0 + 3.0 * 6.0));
    }
    SECTION("直交ベクトルは 0") {
        const Vector3D x{1.0, 0.0, 0.0};
        const Vector3D y{0.0, 1.0, 0.0};
        REQUIRE(x.dot(y) == Catch::Approx(0.0));
    }
    SECTION("自身との内積はノルム二乗") {
        const Vector3D a{3.0, 4.0, 0.0};
        REQUIRE(a.dot(a) == Catch::Approx(25.0));
    }
}
