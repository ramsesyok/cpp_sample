#ifndef MYAPP_COMMON_ECEF_COORDINATE_H_
#define MYAPP_COMMON_ECEF_COORDINATE_H_

#include "common/vector3d.h"

namespace myapp::common {

/**
 * @brief 地球中心地球固定 (ECEF) 座標系の位置。
 *
 * 数学的には Vector3D だが、他の座標系との取り違えを型で防ぐため
 * 独立した型として定義する。Vector3D との暗黙変換は意図的に持たせない。
 */
class EcefCoordinate {
 public:
  /**
   * @brief 各軸の座標値 [m] からインスタンスを生成する。
   * @param x_m  ECEF X 軸成分 [m]。
   * @param y_m  ECEF Y 軸成分 [m]。
   * @param z_m  ECEF Z 軸成分 [m]。
   */
  EcefCoordinate(double x_m, double y_m, double z_m) : v_{x_m, y_m, z_m} {}

  /** @brief X 軸成分 [m]。 */
  double x() const {
    return v_.x;
  }
  /** @brief Y 軸成分 [m]。 */
  double y() const {
    return v_.y;
  }
  /** @brief Z 軸成分 [m]。 */
  double z() const {
    return v_.z;
  }

  /**
   * @brief 2点間の変位ベクトル (this - rhs) を返す。
   * @param rhs  引かれる側の位置。
   * @return 変位ベクトル [m]。
   *
   * 位置同士の加算は意味を持たないため公開しない。
   */
  Vector3D operator-(const EcefCoordinate& rhs) const {
    return v_ - rhs.v_;
  }

  /**
   * @brief 位置に変位ベクトルを加えた新しい位置を返す。
   * @param displacement  加える変位ベクトル [m]。
   * @return 加算後の ECEF 位置。
   */
  EcefCoordinate operator+(const Vector3D& displacement) const {
    return EcefCoordinate{v_.x + displacement.x, v_.y + displacement.y, v_.z + displacement.z};
  }

  /**
   * @brief 位置のスカラー倍を返す (地球中心からの位置ベクトルとして扱う)。
   * @param s  スカラー。
   * @return 各成分を s 倍した ECEF 位置。
   *
   * 位置補間など、線形結合 a * (1 - t) + b * t を構成するために提供する。
   */
  EcefCoordinate operator*(double s) const {
    return EcefCoordinate{v_.x * s, v_.y * s, v_.z * s};
  }

  /**
   * @brief 位置のスカラー除算を返す (地球中心からの位置ベクトルとして扱う)。
   * @param s  ゼロでないスカラー。
   * @return 各成分を s で割った ECEF 位置。
   */
  EcefCoordinate operator/(double s) const {
    return EcefCoordinate{v_.x / s, v_.y / s, v_.z / s};
  }

 private:
  Vector3D v_;
};

}  // namespace myapp::common

#endif  // MYAPP_COMMON_ECEF_COORDINATE_H_
