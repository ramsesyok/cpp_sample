#ifndef MYAPP_COMMON_VECTOR_3D_H
#define MYAPP_COMMON_VECTOR_3D_H

namespace myapp::common {

/**
 * @brief 3次元ベクトル。
 *
 * x_, y_, z_ の独立した3成分を持ち、四則演算 (スカラー演算含む) と
 * 内積を提供する。不変条件を持たないため struct とする。
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

  /** @brief スカラー倍 (this * s)。 */
  constexpr Vector3D operator*(double s) const {
    return {x_ * s, y_ * s, z_ * s};
  }

  /** @brief スカラー除算 (this / s)。 */
  constexpr Vector3D operator/(double s) const {
    return {x_ / s, y_ / s, z_ / s};
  }

  /**
   * @brief 内積を返す。
   * @param rhs  相手側ベクトル。
   * @return 内積値 x·x' + y·y' + z·z'。
   */
  [[nodiscard]] constexpr double Dot(const Vector3D& rhs) const {
    return (x_ * rhs.x_) + (y_ * rhs.y_) + (z_ * rhs.z_);
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
