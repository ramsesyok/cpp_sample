#ifndef MYAPP_COMMON_ECEF_POSITION_H
#define MYAPP_COMMON_ECEF_POSITION_H

#include "common/vector3d.h"

namespace myapp::common {

/**
 * @brief 地球中心地球固定 (ECEF) 座標系の位置。
 *
 * 数学的には Vector3D だが、他の座標系との取り違えを型で防ぐため
 * 独立した型として定義する。Vector3D との暗黙変換は意図的に持たせない。
 */
class EcefPosition {
 public:
  /**
   * @brief 各軸の座標値 [m] からインスタンスを生成する。
   * @param x_m  ECEF X 軸成分 [m]。
   * @param y_m  ECEF Y 軸成分 [m]。
   * @param z_m  ECEF Z 軸成分 [m]。
   */
  EcefPosition(double x_m, double y_m, double z_m) : v_{x_m, y_m, z_m} {}

  /** @brief X 軸成分 [m]。 */
  [[nodiscard]] double X() const {
    return v_.x_;
  }
  /** @brief Y 軸成分 [m]。 */
  [[nodiscard]] double Y() const {
    return v_.y_;
  }
  /** @brief Z 軸成分 [m]。 */
  [[nodiscard]] double Z() const {
    return v_.z_;
  }

  /**
   * @brief 2点間の変位ベクトル (this - rhs) を返す。
   * @param rhs  引かれる側の位置。
   * @return 変位ベクトル [m]。
   *
   * 位置同士の加算は意味を持たないため公開しない。
   */
  Vector3D operator-(const EcefPosition& rhs) const {
    return v_ - rhs.v_;
  }

  /**
   * @brief 位置に変位ベクトルを加えた新しい位置を返す。
   * @param displacement  加える変位ベクトル [m]。
   * @return 加算後の ECEF 位置。
   */
  EcefPosition operator+(const Vector3D& displacement) const {
    return EcefPosition{v_.x_ + displacement.x_, v_.y_ + displacement.y_, v_.z_ + displacement.z_};
  }

 private:
  Vector3D v_;
};

}  // namespace myapp::common

#endif  // MYAPP_COMMON_ECEF_POSITION_H
