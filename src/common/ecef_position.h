#pragma once

#include "common/vector3d.h"

namespace myapp::common {

// 地球中心地球固定 (ECEF: Earth-Centered, Earth-Fixed) 座標系の位置。
// 数学的には Vector3D だが、他の座標系との取り違えを型で防ぐため
// 独立した型として定義する。Vector3D との暗黙変換は意図的に持たせない。
class EcefPosition {
public:
    EcefPosition(double x_m, double y_m, double z_m)
        : v_{x_m, y_m, z_m} {}

    double x() const { return v_.x; }
    double y() const { return v_.y; }
    double z() const { return v_.z; }

    // 2点間の変位ベクトルを返す。位置同士の加算は意味を持たないため公開しない。
    Vector3D operator-(const EcefPosition& rhs) const {
        return v_ - rhs.v_;
    }

    // 位置 + 変位ベクトル = 新しい位置。
    EcefPosition operator+(const Vector3D& displacement) const {
        return EcefPosition{v_.x + displacement.x,
                            v_.y + displacement.y,
                            v_.z + displacement.z};
    }

private:
    Vector3D v_;
};

}  // namespace myapp::common
