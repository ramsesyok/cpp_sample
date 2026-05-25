#ifndef MYAPP_COMMON_VECTOR3D_H_
#define MYAPP_COMMON_VECTOR3D_H_

namespace myapp::common {

/**
 * @brief 3次元ベクトル。
 *
 * x, y, z の独立した3成分を持ち、四則演算 (スカラー演算含む) と
 * 内積を提供する。不変条件を持たないため struct とする。
 */
struct Vector3D {
  double x;
  double y;
  double z;

  /** @brief ベクトル和 (this + rhs)。 */
  constexpr Vector3D operator+(const Vector3D& rhs) const {
    return {x + rhs.x, y + rhs.y, z + rhs.z};
  }

  /** @brief ベクトル差 (this - rhs)。 */
  constexpr Vector3D operator-(const Vector3D& rhs) const {
    return {x - rhs.x, y - rhs.y, z - rhs.z};
  }

  /** @brief スカラー倍 (this * s)。 */
  constexpr Vector3D operator*(double s) const {
    return {x * s, y * s, z * s};
  }

  /** @brief スカラー除算 (this / s)。 */
  constexpr Vector3D operator/(double s) const {
    return {x / s, y / s, z / s};
  }

  /**
   * @brief 内積を返す。
   * @param rhs  相手側ベクトル。
   * @return 内積値 x·x' + y·y' + z·z'。
   */
  constexpr double Dot(const Vector3D& rhs) const {
    return x * rhs.x + y * rhs.y + z * rhs.z;
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

#endif  // MYAPP_COMMON_VECTOR3D_H_
