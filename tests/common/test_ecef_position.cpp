#include <catch2/catch_amalgamated.hpp>
#include <type_traits>

#include "common/ecef_position.h"
#include "common/vector3d.h"

using myapp::common::EcefPosition;
using myapp::common::Vector3D;

TEST_CASE("EcefPosition: コンストラクタとアクセサ", "[ecef]") {
  const EcefPosition kP{1000.0, -2000.0, 3000.0};
  REQUIRE(kP.X() == Catch::Approx(1000.0));
  REQUIRE(kP.Y() == Catch::Approx(-2000.0));
  REQUIRE(kP.Z() == Catch::Approx(3000.0));
}

TEST_CASE("EcefPosition: 位置 - 位置 は変位ベクトル", "[ecef]") {
  const EcefPosition kA{10.0, 20.0, 30.0};
  const EcefPosition kB{1.0, 2.0, 3.0};
  const Vector3D kD = kA - kB;
  REQUIRE(kD.x_ == Catch::Approx(9.0));
  REQUIRE(kD.y_ == Catch::Approx(18.0));
  REQUIRE(kD.z_ == Catch::Approx(27.0));
}

TEST_CASE("EcefPosition: 位置 + 変位 は新しい位置", "[ecef]") {
  const EcefPosition kOrigin{100.0, 200.0, 300.0};
  const Vector3D kDisp{1.0, -2.0, 3.0};
  const EcefPosition kMoved = kOrigin + kDisp;
  REQUIRE(kMoved.X() == Catch::Approx(101.0));
  REQUIRE(kMoved.Y() == Catch::Approx(198.0));
  REQUIRE(kMoved.Z() == Catch::Approx(303.0));
}

// 静的アサーション: 型安全の確認 (コンパイル時に検証される設計上の不変条件)。
// Vector3D <-> EcefPosition の暗黙変換は意図的に許可しない。
static_assert(!std::is_convertible_v<Vector3D, EcefPosition>,
              "Vector3D から EcefPosition への暗黙変換は禁止");
static_assert(!std::is_convertible_v<EcefPosition, Vector3D>,
              "EcefPosition から Vector3D への暗黙変換は禁止");
