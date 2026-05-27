#ifndef MYAPP_COMMON_MATRIX_3X3_H
#define MYAPP_COMMON_MATRIX_3X3_H

#include <cmath>

#include "common/attitude.h"
#include "common/vector3d.h"

namespace myapp::common {

/**
 * @brief 3x3 行列。回転行列および一般の線形変換を表現する。
 *
 * 要素は行優先 (row-major) で mIJ_ (I=行, J=列, 1-origin) に保持する。
 * 行列ベクトル積 M * v はベクトルを列ベクトルとみなして
 * (M * v)_i = Σ_j m_ij * v_j を計算する。
 *
 * 不変条件 (直交性など) は型では保証しない。回転行列として扱う場合は
 * FromAttitude() などの生成関数を用いる。
 */
struct Matrix3x3 {
  double m11_;
  double m12_;
  double m13_;
  double m21_;
  double m22_;
  double m23_;
  double m31_;
  double m32_;
  double m33_;

  /** @brief 単位行列。 */
  static constexpr Matrix3x3 Identity() {
    return Matrix3x3{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  }

  /** @brief 転置行列 (this^T) を返す。 */
  [[nodiscard]] constexpr Matrix3x3 Transpose() const {
    return Matrix3x3{m11_, m21_, m31_, m12_, m22_, m32_, m13_, m23_, m33_};
  }

  /**
   * @brief 行列同士の積 (this * rhs)。
   * @param rhs  右辺の 3x3 行列。
   * @return 積行列。
   */
  [[nodiscard]] constexpr Matrix3x3 operator*(const Matrix3x3& rhs) const {
    return Matrix3x3{
        (m11_ * rhs.m11_) + (m12_ * rhs.m21_) + (m13_ * rhs.m31_),
        (m11_ * rhs.m12_) + (m12_ * rhs.m22_) + (m13_ * rhs.m32_),
        (m11_ * rhs.m13_) + (m12_ * rhs.m23_) + (m13_ * rhs.m33_),
        (m21_ * rhs.m11_) + (m22_ * rhs.m21_) + (m23_ * rhs.m31_),
        (m21_ * rhs.m12_) + (m22_ * rhs.m22_) + (m23_ * rhs.m32_),
        (m21_ * rhs.m13_) + (m22_ * rhs.m23_) + (m23_ * rhs.m33_),
        (m31_ * rhs.m11_) + (m32_ * rhs.m21_) + (m33_ * rhs.m31_),
        (m31_ * rhs.m12_) + (m32_ * rhs.m22_) + (m33_ * rhs.m32_),
        (m31_ * rhs.m13_) + (m32_ * rhs.m23_) + (m33_ * rhs.m33_),
    };
  }

  /**
   * @brief 行列ベクトル積 (this * v)。
   * @param v  3次元ベクトル (列ベクトルとみなす)。
   * @return 変換後のベクトル。
   */
  [[nodiscard]] constexpr Vector3D operator*(const Vector3D& v) const {
    return Vector3D{
        (m11_ * v.x_) + (m12_ * v.y_) + (m13_ * v.z_),
        (m21_ * v.x_) + (m22_ * v.y_) + (m23_ * v.z_),
        (m31_ * v.x_) + (m32_ * v.y_) + (m33_ * v.z_),
    };
  }
};

/**
 * @brief 姿勢角 (Attitude, ZYX オイラー角) から回転行列を生成する。
 *
 * 規約: yaw(psi, Z 軸) → pitch(theta, Y 軸) → roll(phi, X 軸) の順に
 * 内因性回転を適用する航空機標準 (Tait-Bryan ZYX)。すなわち
 *   R = Rz(psi) * Ry(theta) * Rx(phi)
 * となる。R は機体座標系のベクトルを参照座標系 (NED 等) へ変換する
 * 「body → reference」回転として用いることを想定する。
 *
 * @param a  姿勢角 (phi: roll, theta: pitch, psi: yaw, いずれも [rad])。
 * @return 対応する 3x3 回転行列。
 */
/**
 * @brief 行列ベクトル積の自由関数版 (numpy.dot 風)。
 *
 * `m * v` と等価。numpy で行列ベクトル積を `np.dot(M, v)` と書く流儀に
 * 揃えたい場合や、`Dot(...)` で内積・行列積の表記を統一したい場合に使う。
 * @param m  3x3 行列。
 * @param v  3次元ベクトル (列ベクトルとみなす)。
 * @return 変換後のベクトル。
 */
[[nodiscard]] constexpr Vector3D Dot(const Matrix3x3& m, const Vector3D& v) {
  return m * v;
}

inline Matrix3x3 FromAttitude(const Attitude& a) {
  const double kCp = std::cos(a.phi_);
  const double kSp = std::sin(a.phi_);
  const double kCt = std::cos(a.theta_);
  const double kSt = std::sin(a.theta_);
  const double kCs = std::cos(a.psi_);
  const double kSs = std::sin(a.psi_);

  return Matrix3x3{
      kCs * kCt, (kCs * kSt * kSp) - (kSs * kCp), (kCs * kSt * kCp) + (kSs * kSp),
      kSs * kCt, (kSs * kSt * kSp) + (kCs * kCp), (kSs * kSt * kCp) - (kCs * kSp),
      -kSt,      kCt * kSp,                       kCt * kCp,
  };
}

}  // namespace myapp::common

#endif  // MYAPP_COMMON_MATRIX_3X3_H
