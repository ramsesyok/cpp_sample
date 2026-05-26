#ifndef MYAPP_COMMON_BODY_COORDINATE_H
#define MYAPP_COMMON_BODY_COORDINATE_H

#include "common/vector3d.h"

namespace myapp::common {

/**
 * @brief 機体座標系における位置 [m]。
 *
 * オブジェクト (機体) の重心を原点とした右手系の座標で位置を表す。
 * 軸の取り方は航空機慣例に倣い、
 *   x_: 前方 (forward)、y_: 右方 (right)、z_: 下方 (down)
 * を想定するが、本クラス自体は軸の物理的意味に依存しない。
 *
 * GlobalCoordinate / LocalCoordinate と同様にアフィン点として扱い、
 * 位置同士の和やスカラー倍は提供しない。
 */
class BodyCoordinate {
 public:
  /**
   * @brief 各軸の座標値 [m] からインスタンスを生成する。
   * @param x_m  X 軸成分 [m]。
   * @param y_m  Y 軸成分 [m]。
   * @param z_m  Z 軸成分 [m]。
   */
  BodyCoordinate(double x_m, double y_m, double z_m) : v_{x_m, y_m, z_m} {}

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
   */
  Vector3D operator-(const BodyCoordinate& rhs) const {
    return v_ - rhs.v_;
  }

  /**
   * @brief 位置に変位ベクトルを加えた新しい位置を返す。
   * @param displacement  加える変位ベクトル [m]。
   * @return 加算後の機体位置。
   */
  BodyCoordinate operator+(const Vector3D& displacement) const {
    return BodyCoordinate{v_.x_ + displacement.x_, v_.y_ + displacement.y_,
                          v_.z_ + displacement.z_};
  }

  /**
   * @brief 位置から変位ベクトルを引いた新しい位置を返す。
   * @param displacement  引く変位ベクトル [m]。
   * @return 減算後の機体位置。
   */
  BodyCoordinate operator-(const Vector3D& displacement) const {
    return BodyCoordinate{v_.x_ - displacement.x_, v_.y_ - displacement.y_,
                          v_.z_ - displacement.z_};
  }

 private:
  Vector3D v_;
};

}  // namespace myapp::common

#endif  // MYAPP_COMMON_BODY_COORDINATE_H
