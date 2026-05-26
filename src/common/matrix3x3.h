#ifndef MYAPP_COMMON_MATRIX_3X3_H
#define MYAPP_COMMON_MATRIX_3X3_H

#include <cmath>

#include "common/attitude.h"
#include "common/vector3d.h"

namespace myapp::common {

/**
 * @brief 3x3 行列。回転行列および一般の線形変換を表現する。
 *
 * 要素は行優先 (row-major) で m_[row][col] に保持する。
 * 行列ベクトル積 M * v はベクトルを列ベクトルとみなして
 * (M * v)_i = Σ_j m_[i][j] * v_j を計算する。
 *
 * 不変条件 (直交性など) は型では保証しない。回転行列として扱う場合は
 * FromAttitude() などの生成関数を用いる。
 */
struct Matrix3x3 {
  double m_[3][3];

  /** @brief (row, col) 要素への参照。0 ≦ row, col ≦ 2。 */
  constexpr double& At(int row, int col) {
    return m_[row][col];
  }
  /** @brief (row, col) 要素の値。0 ≦ row, col ≦ 2。 */
  [[nodiscard]] constexpr double At(int row, int col) const {
    return m_[row][col];
  }

  /** @brief 単位行列。 */
  static constexpr Matrix3x3 Identity() {
    return Matrix3x3{{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}};
  }

  /** @brief 転置行列 (this^T) を返す。 */
  [[nodiscard]] constexpr Matrix3x3 Transpose() const {
    return Matrix3x3{{{m_[0][0], m_[1][0], m_[2][0]},
                      {m_[0][1], m_[1][1], m_[2][1]},
                      {m_[0][2], m_[1][2], m_[2][2]}}};
  }

  /**
   * @brief 行列同士の積 (this * rhs)。
   * @param rhs  右辺の 3x3 行列。
   * @return 積行列。
   */
  [[nodiscard]] constexpr Matrix3x3 operator*(const Matrix3x3& rhs) const {
    Matrix3x3 r{};
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        r.m_[i][j] = (m_[i][0] * rhs.m_[0][j]) + (m_[i][1] * rhs.m_[1][j]) +
                     (m_[i][2] * rhs.m_[2][j]);
      }
    }
    return r;
  }

  /**
   * @brief 行列ベクトル積 (this * v)。
   * @param v  3次元ベクトル (列ベクトルとみなす)。
   * @return 変換後のベクトル。
   */
  [[nodiscard]] constexpr Vector3D operator*(const Vector3D& v) const {
    return Vector3D{
        (m_[0][0] * v.x_) + (m_[0][1] * v.y_) + (m_[0][2] * v.z_),
        (m_[1][0] * v.x_) + (m_[1][1] * v.y_) + (m_[1][2] * v.z_),
        (m_[2][0] * v.x_) + (m_[2][1] * v.y_) + (m_[2][2] * v.z_),
    };
  }
};

/**
 * @brief 姿勢角 (Attitude, ZYX オイラー角) から回転行列を生成する。
 *
 * 規約: yaw(psi, Z 軸) → pitch(theta, Y 軸) → roll(phi, X 軸) の順に
 * 内因性回転を適用する航空機標準 (Tait-Bryan ZYX)。すなわち
 *   R = Rz(psi) * Ry(theta) * Rx(phi)
 * となる。R は機体座標系のベクトルを参照座標系 (NEU 等) へ変換する
 * 「body → reference」回転として用いることを想定する。
 *
 * @param a  姿勢角 (phi: roll, theta: pitch, psi: yaw, いずれも [rad])。
 * @return 対応する 3x3 回転行列。
 */
inline Matrix3x3 FromAttitude(const Attitude& a) {
  const double kCp = std::cos(a.phi_);
  const double kSp = std::sin(a.phi_);
  const double kCt = std::cos(a.theta_);
  const double kSt = std::sin(a.theta_);
  const double kCs = std::cos(a.psi_);
  const double kSs = std::sin(a.psi_);

  return Matrix3x3{{
      {kCs * kCt, (kCs * kSt * kSp) - (kSs * kCp), (kCs * kSt * kCp) + (kSs * kSp)},
      {kSs * kCt, (kSs * kSt * kSp) + (kCs * kCp), (kSs * kSt * kCp) - (kCs * kSp)},
      {-kSt, kCt * kSp, kCt * kCp},
  }};
}

}  // namespace myapp::common

#endif  // MYAPP_COMMON_MATRIX_3X3_H
