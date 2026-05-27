#ifndef MYAPP_COMMON_LOCAL_COORDINATE_H
#define MYAPP_COMMON_LOCAL_COORDINATE_H

#include "common/vector3d.h"

namespace myapp::common {

/**
 * @brief ローカル (NED: North-East-Down) 接平面座標系における位置 [m]。
 *
 * 参照点 (通常は ECEF 上のある GeoCoordinate) を原点とし、
 *   x_: 北 (North)、y_: 東 (East)、z_: 下 (Down)
 * の右手系で位置を表す。本プロジェクト内では「ローカル座標系」と呼ぶ。
 *
 * GlobalCoordinate と同様にアフィン点として扱い、位置同士の和や
 * スカラー倍は提供しない。位置 - 位置 = 変位ベクトル、
 * 位置 ± 変位ベクトル = 位置 のみを許可する。
 */
class LocalCoordinate {
 public:
  /**
   * @brief 各軸の座標値 [m] からインスタンスを生成する。
   * @param x_m  北方向成分 [m]。
   * @param y_m  東方向成分 [m]。
   * @param z_m  下方向成分 [m]。
   */
  LocalCoordinate(double x_m, double y_m, double z_m) : v_{x_m, y_m, z_m} {}

  /** @brief 北方向成分 [m]。 */
  [[nodiscard]] double X() const {
    return v_.x_;
  }
  /** @brief 東方向成分 [m]。 */
  [[nodiscard]] double Y() const {
    return v_.y_;
  }
  /** @brief 下方向成分 [m]。 */
  [[nodiscard]] double Z() const {
    return v_.z_;
  }

  /**
   * @brief 2点間の変位ベクトル (this - rhs) を返す。
   * @param rhs  引かれる側の位置。
   * @return 変位ベクトル [m]。
   */
  Vector3D operator-(const LocalCoordinate& rhs) const {
    return v_ - rhs.v_;
  }

  /**
   * @brief 位置に変位ベクトルを加えた新しい位置を返す。
   * @param displacement  加える変位ベクトル [m]。
   * @return 加算後のローカル位置。
   */
  LocalCoordinate operator+(const Vector3D& displacement) const {
    return LocalCoordinate{v_.x_ + displacement.x_, v_.y_ + displacement.y_,
                           v_.z_ + displacement.z_};
  }

  /**
   * @brief 位置から変位ベクトルを引いた新しい位置を返す。
   * @param displacement  引く変位ベクトル [m]。
   * @return 減算後のローカル位置。
   */
  LocalCoordinate operator-(const Vector3D& displacement) const {
    return LocalCoordinate{v_.x_ - displacement.x_, v_.y_ - displacement.y_,
                           v_.z_ - displacement.z_};
  }

 private:
  Vector3D v_;
};

}  // namespace myapp::common

#endif  // MYAPP_COMMON_LOCAL_COORDINATE_H
