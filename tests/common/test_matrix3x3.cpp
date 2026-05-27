#include <catch2/catch_amalgamated.hpp>

#include <cmath>

#include "common/attitude.h"
#include "common/matrix3x3.h"
#include "common/vector3d.h"

using myapp::common::Attitude;
using myapp::common::Dot;
using myapp::common::FromAttitude;
using myapp::common::Matrix3x3;
using myapp::common::Vector3D;

namespace {
constexpr double kPi = 3.14159265358979323846;
constexpr double kHalfPi = kPi / 2.0;

// 2 つの行列が要素ごとに `Catch::Approx(margin)` の意味で等しいことを確認する。
// 9 個の REQUIRE をテスト本体に直書きすると認知的複雑度が閾値を超えるため、
// ここに集約する。
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
void RequireMatrixApprox(const Matrix3x3& actual, const Matrix3x3& expected,
                         double margin = 0.0) {
  REQUIRE(actual.m11_ == Catch::Approx(expected.m11_).margin(margin));
  REQUIRE(actual.m12_ == Catch::Approx(expected.m12_).margin(margin));
  REQUIRE(actual.m13_ == Catch::Approx(expected.m13_).margin(margin));
  REQUIRE(actual.m21_ == Catch::Approx(expected.m21_).margin(margin));
  REQUIRE(actual.m22_ == Catch::Approx(expected.m22_).margin(margin));
  REQUIRE(actual.m23_ == Catch::Approx(expected.m23_).margin(margin));
  REQUIRE(actual.m31_ == Catch::Approx(expected.m31_).margin(margin));
  REQUIRE(actual.m32_ == Catch::Approx(expected.m32_).margin(margin));
  REQUIRE(actual.m33_ == Catch::Approx(expected.m33_).margin(margin));
}
}  // namespace

TEST_CASE("Matrix3x3: Identity は単位行列", "[matrix3x3]") {
  RequireMatrixApprox(Matrix3x3::Identity(), Matrix3x3{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0});
}

TEST_CASE("Matrix3x3: Identity * v == v", "[matrix3x3]") {
  const Matrix3x3 kI = Matrix3x3::Identity();
  const Vector3D kV{1.0, -2.0, 3.5};
  const Vector3D kR = kI * kV;
  REQUIRE(kR.x_ == Catch::Approx(1.0));
  REQUIRE(kR.y_ == Catch::Approx(-2.0));
  REQUIRE(kR.z_ == Catch::Approx(3.5));
}

TEST_CASE("Matrix3x3: Transpose", "[matrix3x3]") {
  const Matrix3x3 kM{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
  const Matrix3x3 kExpected{1.0, 4.0, 7.0, 2.0, 5.0, 8.0, 3.0, 6.0, 9.0};
  RequireMatrixApprox(kM.Transpose(), kExpected);
}

TEST_CASE("Matrix3x3: 行列同士の積", "[matrix3x3]") {
  const Matrix3x3 kA{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
  const Matrix3x3 kI = Matrix3x3::Identity();
  RequireMatrixApprox(kA * kI, kA);
}

TEST_CASE("Matrix3x3: 行列ベクトル積", "[matrix3x3]") {
  // 単純な対角行列で各成分のスケーリングを確認。
  const Matrix3x3 kS{2.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 4.0};
  const Vector3D kV{1.0, 1.0, 1.0};
  const Vector3D kR = kS * kV;
  REQUIRE(kR.x_ == Catch::Approx(2.0));
  REQUIRE(kR.y_ == Catch::Approx(3.0));
  REQUIRE(kR.z_ == Catch::Approx(4.0));

  SECTION("自由関数版 Dot(m, v) は m * v と等価") {
    const Vector3D kRd = Dot(kS, kV);
    REQUIRE(kRd == kR);
  }
}

TEST_CASE("FromAttitude: ゼロ姿勢は単位行列", "[matrix3x3]") {
  RequireMatrixApprox(FromAttitude(Attitude{0.0, 0.0, 0.0}), Matrix3x3::Identity(), 1e-12);
}

TEST_CASE("FromAttitude: yaw 90度は X 軸を Y 軸に写す", "[matrix3x3]") {
  // ZYX 規約: Rz(pi/2) は (1,0,0) -> (0,1,0)。
  const Matrix3x3 kR = FromAttitude(Attitude{0.0, 0.0, kHalfPi});
  const Vector3D kV = kR * Vector3D{1.0, 0.0, 0.0};
  REQUIRE(kV.x_ == Catch::Approx(0.0).margin(1e-12));
  REQUIRE(kV.y_ == Catch::Approx(1.0).margin(1e-12));
  REQUIRE(kV.z_ == Catch::Approx(0.0).margin(1e-12));
}

TEST_CASE("FromAttitude: pitch 90度は X 軸を -Z 軸に写す", "[matrix3x3]") {
  // ZYX 規約: Ry(pi/2) は (1,0,0) -> (0,0,-1)。
  const Matrix3x3 kR = FromAttitude(Attitude{0.0, kHalfPi, 0.0});
  const Vector3D kV = kR * Vector3D{1.0, 0.0, 0.0};
  REQUIRE(kV.x_ == Catch::Approx(0.0).margin(1e-12));
  REQUIRE(kV.y_ == Catch::Approx(0.0).margin(1e-12));
  REQUIRE(kV.z_ == Catch::Approx(-1.0).margin(1e-12));
}

TEST_CASE("FromAttitude: roll 90度は Y 軸を Z 軸に写す", "[matrix3x3]") {
  // ZYX 規約: Rx(pi/2) は (0,1,0) -> (0,0,1)。
  const Matrix3x3 kR = FromAttitude(Attitude{kHalfPi, 0.0, 0.0});
  const Vector3D kV = kR * Vector3D{0.0, 1.0, 0.0};
  REQUIRE(kV.x_ == Catch::Approx(0.0).margin(1e-12));
  REQUIRE(kV.y_ == Catch::Approx(0.0).margin(1e-12));
  REQUIRE(kV.z_ == Catch::Approx(1.0).margin(1e-12));
}

TEST_CASE("FromAttitude: 回転行列は直交 (R * R^T == I)", "[matrix3x3]") {
  const Matrix3x3 kR = FromAttitude(Attitude{0.3, -0.7, 1.2});
  RequireMatrixApprox(kR * kR.Transpose(), Matrix3x3::Identity(), 1e-12);
}
