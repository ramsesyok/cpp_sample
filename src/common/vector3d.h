#ifndef MYAPP_COMMON_VECTOR_3D_H
#define MYAPP_COMMON_VECTOR_3D_H

#include <cmath>

namespace myapp::common {

/**
 * @brief 3次元ベクトル。
 *
 * x_, y_, z_ の独立した3成分を持ち、四則演算 (スカラー演算含む) と
 * 内積・外積・ノルム関連を提供する。不変条件を持たないため struct とする。
 *
 * 数値シミュレーションのループでは状態の積算 (`v += a * dt` 等) が頻出する
 * ため、複合代入 (`+=`, `-=`, `*=`, `/=`) と単項マイナスも提供する。
 *
 * 注意: 本型はフレーム情報を持たない。ECEF/NED/Body の取り違えに
 * よるバグは型では防げないため、利用側のコーディング規約で吸収する。
 */
struct Vector3D {
  double x_;
  double y_;
  double z_;

  /** @brief ベクトル和 (this + rhs)。 */
  constexpr Vector3D operator+(const Vector3D& rhs) const {
    return {x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_};
  }

  /** @brief ベクトル差 (this - rhs)。 */
  constexpr Vector3D operator-(const Vector3D& rhs) const {
    return {x_ - rhs.x_, y_ - rhs.y_, z_ - rhs.z_};
  }

  /** @brief 単項マイナス (符号反転)。 */
  constexpr Vector3D operator-() const {
    return {-x_, -y_, -z_};
  }

  /** @brief スカラー倍 (this * s)。 */
  constexpr Vector3D operator*(double s) const {
    return {x_ * s, y_ * s, z_ * s};
  }

  /** @brief スカラー除算 (this / s)。 */
  constexpr Vector3D operator/(double s) const {
    return {x_ / s, y_ / s, z_ / s};
  }

  /** @brief 複合代入: 加算。 */
  constexpr Vector3D& operator+=(const Vector3D& rhs) {
    x_ += rhs.x_;
    y_ += rhs.y_;
    z_ += rhs.z_;
    return *this;
  }

  /** @brief 複合代入: 減算。 */
  constexpr Vector3D& operator-=(const Vector3D& rhs) {
    x_ -= rhs.x_;
    y_ -= rhs.y_;
    z_ -= rhs.z_;
    return *this;
  }

  /** @brief 複合代入: スカラー倍。 */
  constexpr Vector3D& operator*=(double s) {
    x_ *= s;
    y_ *= s;
    z_ *= s;
    return *this;
  }

  /** @brief 複合代入: スカラー除算。 */
  constexpr Vector3D& operator/=(double s) {
    x_ /= s;
    y_ /= s;
    z_ /= s;
    return *this;
  }

  /** @brief 厳密一致比較 (浮動小数の bit-for-bit 比較)。許容誤差付き比較は別途必要。 */
  constexpr bool operator==(const Vector3D& rhs) const {
    return x_ == rhs.x_ && y_ == rhs.y_ && z_ == rhs.z_;
  }

  /** @brief 厳密一致比較の否定。 */
  constexpr bool operator!=(const Vector3D& rhs) const {
    return !(*this == rhs);
  }

  /**
   * @brief 内積を返す。
   * @param rhs  相手側ベクトル。
   * @return 内積値 x·x' + y·y' + z·z'。
   */
  [[nodiscard]] constexpr double Dot(const Vector3D& rhs) const {
    return (x_ * rhs.x_) + (y_ * rhs.y_) + (z_ * rhs.z_);
  }

  /**
   * @brief 外積を返す (this × rhs)。
   * @param rhs  相手側ベクトル。
   * @return 外積ベクトル。右手系の慣例に従う。
   */
  [[nodiscard]] constexpr Vector3D Cross(const Vector3D& rhs) const {
    return Vector3D{
        (y_ * rhs.z_) - (z_ * rhs.y_),
        (z_ * rhs.x_) - (x_ * rhs.z_),
        (x_ * rhs.y_) - (y_ * rhs.x_),
    };
  }

  /** @brief ノルム二乗 (= Dot(self))。平方根なしで距離比較したいときに使う。 */
  [[nodiscard]] constexpr double SquaredNorm() const {
    return (x_ * x_) + (y_ * y_) + (z_ * z_);
  }

  /** @brief ノルム (ユークリッド長)。 */
  [[nodiscard]] double Norm() const {
    return std::sqrt(SquaredNorm());
  }

  /**
   * @brief 単位ベクトル化したコピーを返す。
   *
   * ノルムが 0 のベクトルに対しては (0, 0, 0) を返す (NaN を返さない)。
   * 呼び出し側がゼロベクトルを扱わない前提なら、Norm() を直接見て自分で
   * チェックすることを推奨する。
   */
  [[nodiscard]] Vector3D Normalized() const {
    const double kN = Norm();
    if (kN == 0.0) {
      return Vector3D{0.0, 0.0, 0.0};
    }
    return Vector3D{x_ / kN, y_ / kN, z_ / kN};
  }
};

/**
 * @brief スカラー左辺のスカラー倍 (s * v)。
 * @param s  スカラー。
 * @param v  ベクトル。
 * @return s 倍されたベクトル。
 */
constexpr Vector3D operator*(double s, const Vector3D& v) {
  return v * s;
}

}  // namespace myapp::common

#endif  // MYAPP_COMMON_VECTOR_3D_H
