#include <catch2/catch_amalgamated.hpp>

#include <type_traits>

#include "common/ecef_position.h"
#include "common/vector3d.h"

using myapp::common::EcefPosition;
using myapp::common::Vector3D;

TEST_CASE("EcefPosition: コンストラクタとアクセサ", "[ecef]") {
  const EcefPosition p{1000.0, -2000.0, 3000.0};
  REQUIRE(p.x() == Catch::Approx(1000.0));
  REQUIRE(p.y() == Catch::Approx(-2000.0));
  REQUIRE(p.z() == Catch::Approx(3000.0));
}

TEST_CASE("EcefPosition: 位置 - 位置 は変位ベクトル", "[ecef]") {
  const EcefPosition a{10.0, 20.0, 30.0};
  const EcefPosition b{1.0, 2.0, 3.0};
  const Vector3D d = a - b;
  REQUIRE(d.x == Catch::Approx(9.0));
  REQUIRE(d.y == Catch::Approx(18.0));
  REQUIRE(d.z == Catch::Approx(27.0));
}

TEST_CASE("EcefPosition: 位置 + 変位 は新しい位置", "[ecef]") {
  const EcefPosition origin{100.0, 200.0, 300.0};
  const Vector3D disp{1.0, -2.0, 3.0};
  const EcefPosition moved = origin + disp;
  REQUIRE(moved.x() == Catch::Approx(101.0));
  REQUIRE(moved.y() == Catch::Approx(198.0));
  REQUIRE(moved.z() == Catch::Approx(303.0));
}

// 静的アサーション: 型安全の確認 (コンパイル時に検証される設計上の不変条件)。
// Vector3D <-> EcefPosition の暗黙変換は意図的に許可しない。
static_assert(!std::is_convertible_v<Vector3D, EcefPosition>,
              "Vector3D から EcefPosition への暗黙変換は禁止");
static_assert(!std::is_convertible_v<EcefPosition, Vector3D>,
              "EcefPosition から Vector3D への暗黙変換は禁止");
