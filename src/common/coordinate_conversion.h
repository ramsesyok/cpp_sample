#ifndef MYAPP_COMMON_COORDINATE_CONVERSION_H
#define MYAPP_COMMON_COORDINATE_CONVERSION_H

#include <cmath>

#include "common/ecef_position.h"
#include "common/geo_coordinate.h"
#include "common/map_coordinate.h"

namespace myapp::common {

// 座標系間の明示的な変換関数群。
// 暗黙変換を許さないことが設計上の意図なので、変換は常にこの関数経由で
// 明示する。
//
// 楕円体は WGS84 を使用する。
// EcefToGeo は閉形式 (Bowring 1976) を採用。地表近傍 (~航空機高度) で
// 数値誤差レベルの精度が反復なしで得られる。極や軌道高度を多用する場合
// は Heikkinen の閉形式に差し替えるとよい。

namespace wgs84 {

/** @brief WGS84 楕円体の長半径 [m]。 */
inline constexpr double kSemiMajorAxisM = 6378137.0;
/** @brief WGS84 楕円体の扁平率 f = 1/298.257223563。 */
inline constexpr double kFlattening = 1.0 / 298.257223563;
/** @brief 第一離心率の二乗 e² = 2f - f²。 */
inline constexpr double kFirstEccentricitySq = kFlattening * (2.0 - kFlattening);

}  // namespace wgs84

/**
 * @brief GeoCoordinate (rad) を MapCoordinate (deg) に変換する。
 * @param g  ラジアン基準のジオデティック座標。
 * @return 度基準の地図座標。
 */
inline MapCoordinate ToMap(const GeoCoordinate& g) {
  // 180 / pi。pi は M_PI が処理系依存なので定数を直書きする。
  constexpr double kRadToDeg = 57.29577951308232087679815481410517;
  return MapCoordinate{
      g.Latitude() * kRadToDeg,
      g.Longitude() * kRadToDeg,
      g.Altitude(),
  };
}

/**
 * @brief MapCoordinate (deg) を GeoCoordinate (rad) に変換する。
 * @param m  度基準の地図座標。
 * @return ラジアン基準のジオデティック座標。
 */
inline GeoCoordinate ToGeo(const MapCoordinate& m) {
  // pi / 180。
  constexpr double kDegToRad = 0.01745329251994329576923690768488613;
  return GeoCoordinate{
      m.LatitudeDeg() * kDegToRad,
      m.LongitudeDeg() * kDegToRad,
      m.AltitudeM(),
  };
}

/**
 * @brief WGS84 ジオデティック座標を ECEF 座標 [m] に変換する。
 *
 * 標準式:
 * @code
 *   N(φ) = a / sqrt(1 - e² sin²φ)
 *   X = (N + h) cosφ cosλ
 *   Y = (N + h) cosφ sinλ
 *   Z = (N(1 - e²) + h) sinφ
 * @endcode
 *
 * 入力は GeoCoordinate (ラジアン基準) 限定。度入力は ToGeo() で
 * 一度ラジアンに変換してから渡す。
 *
 * @param g  ラジアン基準のジオデティック座標。
 * @return WGS84 楕円体上の ECEF 位置 [m]。
 */
inline EcefPosition GeoToEcef(const GeoCoordinate& g) {
  const double kSinLat = std::sin(g.Latitude());
  const double kCosLat = std::cos(g.Latitude());
  const double kSinLon = std::sin(g.Longitude());
  const double kCosLon = std::cos(g.Longitude());

  const double kN =
      wgs84::kSemiMajorAxisM / std::sqrt(1.0 - (wgs84::kFirstEccentricitySq * kSinLat * kSinLat));
  const double kNPlusH = kN + g.Altitude();

  const double kX = kNPlusH * kCosLat * kCosLon;
  const double kY = kNPlusH * kCosLat * kSinLon;
  const double kZ = ((kN * (1.0 - wgs84::kFirstEccentricitySq)) + g.Altitude()) * kSinLat;

  return EcefPosition{kX, kY, kZ};
}

/**
 * @brief ECEF 座標 [m] を WGS84 ジオデティック座標に変換する (Bowring 1976)。
 *
 * 経度は λ = atan2(Y, X) で常に厳密。緯度 φ は補助緯度 β を経由する閉形式:
 * @code
 *   tan β = (Z · a) / (p · b)              ※ p = sqrt(X² + Y²)
 *   tan φ = (Z + e'² · b · sin³β) / (p - e² · a · cos³β)
 * @endcode
 * 高度は h = p / cosφ - N で求めるが、極近傍では cosφ ≈ 0 となるため
 * 代替式 h = |Z| / |sinφ| - N(1 - e²) に切り替える。
 *
 * @param p_ecef  ECEF 位置 [m]。
 * @return WGS84 楕円体上のジオデティック座標 (ラジアン基準)。
 */
inline GeoCoordinate EcefToGeo(const EcefPosition& p_ecef) {
  constexpr double kA = wgs84::kSemiMajorAxisM;
  constexpr double kF = wgs84::kFlattening;
  constexpr double kE2 = wgs84::kFirstEccentricitySq;
  constexpr double kB = kA * (1.0 - kF);      // 短半径
  constexpr double kEp2 = kE2 / (1.0 - kE2);  // 第二離心率の二乗 e'²
  constexpr double kHalfPi = 1.5707963267948966;

  const double kX = p_ecef.X();
  const double kY = p_ecef.Y();
  const double kZ = p_ecef.Z();

  const double kP = std::sqrt((kX * kX) + (kY * kY));
  const double kLon = std::atan2(kY, kX);

  // 自転軸上 (p ≈ 0): 経度は不定なので 0 を返し、緯度は ±π/2、
  // 高度は Z 軸上での原点からの距離から短半径 b を引いた値。
  if (kP == 0.0) {
    const double kLatPole = (kZ >= 0.0) ? kHalfPi : -kHalfPi;
    const double kHPole = std::abs(kZ) - kB;
    return GeoCoordinate{kLatPole, 0.0, kHPole};
  }

  // 補助緯度 β の初期推定。
  const double kBeta = std::atan2(kZ * kA, kP * kB);
  const double kSinBeta = std::sin(kBeta);
  const double kCosBeta = std::cos(kBeta);

  // Bowring の閉形式による緯度 φ。
  const double kLat = std::atan2(kZ + (kEp2 * kB * kSinBeta * kSinBeta * kSinBeta),
                                 kP - (kE2 * kA * kCosBeta * kCosBeta * kCosBeta));

  const double kSinLat = std::sin(kLat);
  const double kCosLat = std::cos(kLat);
  const double kN = kA / std::sqrt(1.0 - (kE2 * kSinLat * kSinLat));

  // 高度。極近傍では p/cosφ が発散するため、Z 軸方向の式に切り替える。
  double h = 0.0;
  if (std::abs(kCosLat) > 1e-10) {
    h = (kP / kCosLat) - kN;
  } else {
    h = (std::abs(kZ) / std::abs(kSinLat)) - (kN * (1.0 - kE2));
  }

  return GeoCoordinate{kLat, kLon, h};
}

}  // namespace myapp::common

#endif  // MYAPP_COMMON_COORDINATE_CONVERSION_H
