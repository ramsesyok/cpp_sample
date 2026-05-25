#pragma once

namespace myapp::common {

struct Vector3D {
    double x;
    double y;
    double z;

    constexpr Vector3D operator+(const Vector3D& rhs) const {
        return {x + rhs.x, y + rhs.y, z + rhs.z};
    }
    constexpr Vector3D operator-(const Vector3D& rhs) const {
        return {x - rhs.x, y - rhs.y, z - rhs.z};
    }
    constexpr Vector3D operator*(double s) const {
        return {x * s, y * s, z * s};
    }
    constexpr Vector3D operator/(double s) const {
        return {x / s, y / s, z / s};
    }

    constexpr double dot(const Vector3D& rhs) const {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }
};

constexpr Vector3D operator*(double s, const Vector3D& v) {
    return v * s;
}

}  // namespace myapp::common
