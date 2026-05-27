#ifndef MYAPP_COMMON_CONSTANTS_H
#define MYAPP_COMMON_CONSTANTS_H

namespace myapp::common {

/**
 * @brief 円周率 π。
 *
 * M_PI は処理系依存 (POSIX 拡張) なため、プロジェクト共通の定数として
 * ここに集約する。倍精度の表現可能な範囲で π に最も近い値。
 */
inline constexpr double kPi = 3.14159265358979323846;

/** @brief π / 2。 */
inline constexpr double kHalfPi = kPi / 2.0;

/** @brief 2π。 */
inline constexpr double kTwoPi = kPi * 2.0;

/** @brief ラジアン → 度 への係数 (= 180 / π)。 */
inline constexpr double kRadToDeg = 57.29577951308232087679815481410517;

/** @brief 度 → ラジアン への係数 (= π / 180)。 */
inline constexpr double kDegToRad = 0.01745329251994329576923690768488613;

}  // namespace myapp::common

#endif  // MYAPP_COMMON_CONSTANTS_H
