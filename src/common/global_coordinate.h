#ifndef MYAPP_COMMON_GLOBAL_COORDINATE_H
#define MYAPP_COMMON_GLOBAL_COORDINATE_H

#include "common/vector3d.h"

namespace myapp::common {

/**
 * @brief グローバル座標系 (ECEF: Earth-Centered, Earth-Fixed) の位置 [m]。
 *
 * 地球中心地球固定座標系 (ECEF) を本プロジェクトでは「グローバル座標系」と呼ぶ。
 * 参照楕円体は WGS84 を用いる。x, y, z はそれぞれ ECEF X/Y/Z 軸成分 [m] である。
 *
 * 数学的には Vector3D だが、他の座標系 (ローカル/機体) との取り違えを型で
 * 防ぐため独立した型として定義する。Vector3D との暗黙変換は意図的に持たせない。
 */
class GlobalCoordinate {
 public:
  /**
   * @brief 各軸の座標値 [m] からインスタンスを生成する。
   * @param x_m  ECEF X 軸成分 [m]。
   * @param y_m  ECEF Y 軸成分 [m]。
   * @param z_m  ECEF Z 軸成分 [m]。
   */
  GlobalCoordinate(double x_m, double y_m, double z_m) : v_{x_m, y_m, z_m} {}

  /** @brief ECEF X 軸成分 [m]。 */
  [[nodiscard]] double X() const {
    return v_.x_;
  }
  /** @brief ECEF Y 軸成分 [m]。 */
  [[nodiscard]] double Y() const {
    return v_.y_;
  }
  /** @brief ECEF Z 軸成分 [m]。 */
  [[nodiscard]] double Z() const {
    return v_.z_;
  }

  /**
   * @brief 2点間の変位ベクトル (this - rhs) を返す。
   * @param rhs  引かれる側の ECEF 位置。
   * @return 変位ベクトル [m]。
   *
   * 位置同士の加算は意味を持たないため公開しない。
   */
  Vector3D operator-(const GlobalCoordinate& rhs) const {
    return v_ - rhs.v_;
  }

  /**
   * @brief 位置に変位ベクトルを加えた新しい位置を返す。
   * @param displacement  加える変位ベクトル [m]。
   * @return 加算後の ECEF 位置。
   */
  GlobalCoordinate operator+(const Vector3D& displacement) const {
    const Vector3D kR = v_ + displacement;
    return GlobalCoordinate{kR.x_, kR.y_, kR.z_};
  }

  /**
   * @brief 位置から変位ベクトルを引いた新しい位置を返す。
   * @param displacement  引く変位ベクトル [m]。
   * @return 減算後の ECEF 位置。
   */
  GlobalCoordinate operator-(const Vector3D& displacement) const {
    const Vector3D kR = v_ - displacement;
    return GlobalCoordinate{kR.x_, kR.y_, kR.z_};
  }

 private:
  Vector3D v_;
};

}  // namespace myapp::common

#endif  // MYAPP_COMMON_GLOBAL_COORDINATE_H
