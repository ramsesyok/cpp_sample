#ifndef MYAPP_COMMON_COORDINATE_CONVERSION_H
#define MYAPP_COMMON_COORDINATE_CONVERSION_H

#include <cmath>

#include "common/attitude.h"
#include "common/body_coordinate.h"
#include "common/constants.h"
#include "common/geo_coordinate.h"
#include "common/global_coordinate.h"
#include "common/local_coordinate.h"
#include "common/map_coordinate.h"
#include "common/matrix3x3.h"
#include "common/vector3d.h"

namespace myapp::common {

// 座標系間の明示的な変換関数群。
// 暗黙変換を許さないことが設計上の意図なので、変換は常にこの関数経由で
// 明示する。
//
// 楕円体は WGS84 を使用する。
// GlobalToGeo は閉形式 (Bowring 1976) を採用。地表近傍 (~航空機高度) で
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
inline MapCoordinate GeoToMap(const GeoCoordinate& g) {
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
inline GeoCoordinate MapToGeo(const MapCoordinate& m) {
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
 * 入力は GeoCoordinate (ラジアン基準) 限定。度入力は MapToGeo() で
 * 一度ラジアンに変換してから渡す。
 *
 * @param g  ラジアン基準のジオデティック座標。
 * @return WGS84 楕円体上の ECEF 位置 [m]。
 */
inline GlobalCoordinate GeoToGlobal(const GeoCoordinate& g) {
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

  return GlobalCoordinate{kX, kY, kZ};
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
 * 自転軸近傍 (p が極小) では Bowring の式が数値的に不安定になるため、
 * 閾値以下なら極上にあるとみなす特別経路を取る。
 *
 * @param p_ecef  ECEF 位置 [m]。
 * @return WGS84 楕円体上のジオデティック座標 (ラジアン基準)。
 */
inline GeoCoordinate GlobalToGeo(const GlobalCoordinate& p_ecef) {
  constexpr double kA = wgs84::kSemiMajorAxisM;
  constexpr double kF = wgs84::kFlattening;
  constexpr double kE2 = wgs84::kFirstEccentricitySq;
  constexpr double kB = kA * (1.0 - kF);      // 短半径
  constexpr double kEp2 = kE2 / (1.0 - kE2);  // 第二離心率の二乗 e'²

  // 自転軸近傍の閾値 [m]。地球半径 (~6.378e6 m) に対して 1mm 相当。
  // この内側では Bowring の atan2(Z·a, p·b) が p≈0 由来でノイズを増幅
  // するため、極ケースとして処理する。
  constexpr double kAxisEpsilonM = 1e-3;

  const double kX = p_ecef.X();
  const double kY = p_ecef.Y();
  const double kZ = p_ecef.Z();

  const double kP = std::sqrt((kX * kX) + (kY * kY));
  const double kLon = std::atan2(kY, kX);

  // 自転軸上 (p ≈ 0): 経度は不定なので 0 を返し、緯度は ±π/2、
  // 高度は Z 軸上での原点からの距離から短半径 b を引いた値。
  if (kP < kAxisEpsilonM) {
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

// =====================================================================
// NED (LocalCoordinate) ⇔ ECEF (GlobalCoordinate)
// =====================================================================
//
// 参照点 (lat, lon) を原点とする NED 接平面座標系と ECEF の間の変換。
// 回転行列 R_ne (NED → ECEF) は以下で与えられる:
//
//   R_ne = [ -sinφ cosλ,  -sinλ,  -cosφ cosλ ]
//          [ -sinφ sinλ,   cosλ,  -cosφ sinλ ]
//          [        cosφ,     0,        -sinφ ]
//
// (列はそれぞれ NED の N, E, D 軸を ECEF 成分で表したもの)。
// ECEF → NED への変換は R_ne の転置を使う (回転行列なので)。

/**
 * @brief 参照点での NED → ECEF 回転行列を返す。
 * @param ref_geo  接平面の原点となるジオデティック座標 (rad)。
 * @return NED 軸基底を ECEF 成分で表現した 3x3 回転行列。
 */
inline Matrix3x3 NedToGlobalRotation(const GeoCoordinate& ref_geo) {
  const double kSinLat = std::sin(ref_geo.Latitude());
  const double kCosLat = std::cos(ref_geo.Latitude());
  const double kSinLon = std::sin(ref_geo.Longitude());
  const double kCosLon = std::cos(ref_geo.Longitude());

  return Matrix3x3{
      -kSinLat * kCosLon, -kSinLon, -kCosLat * kCosLon,
      -kSinLat * kSinLon,  kCosLon, -kCosLat * kSinLon,
       kCosLat,            0.0,     -kSinLat,
  };
}

/**
 * @brief 参照点での ECEF → NED 回転行列を返す (NedToGlobalRotation の転置)。
 * @param ref_geo  接平面の原点となるジオデティック座標 (rad)。
 */
inline Matrix3x3 GlobalToNedRotation(const GeoCoordinate& ref_geo) {
  return NedToGlobalRotation(ref_geo).Transpose();
}

/**
 * @brief NED (LocalCoordinate) の位置を ECEF (GlobalCoordinate) に変換する。
 *
 * @param local    参照点を原点とする NED 位置 [m]。
 * @param ref_geo  接平面の原点となるジオデティック座標 (rad)。
 * @return ECEF 位置 [m]。
 */
inline GlobalCoordinate LocalToGlobal(const LocalCoordinate& local, const GeoCoordinate& ref_geo) {
  const GlobalCoordinate kRefEcef = GeoToGlobal(ref_geo);
  const Vector3D kLocalVec{local.X(), local.Y(), local.Z()};
  const Vector3D kEcefDelta = NedToGlobalRotation(ref_geo) * kLocalVec;
  return kRefEcef + kEcefDelta;
}

/**
 * @brief ECEF (GlobalCoordinate) の位置を参照点中心の NED (LocalCoordinate) に変換する。
 *
 * @param p_ecef   変換したい ECEF 位置 [m]。
 * @param ref_geo  接平面の原点となるジオデティック座標 (rad)。
 * @return 参照点を原点とした NED 位置 [m]。
 */
inline LocalCoordinate GlobalToLocal(const GlobalCoordinate& p_ecef, const GeoCoordinate& ref_geo) {
  const GlobalCoordinate kRefEcef = GeoToGlobal(ref_geo);
  const Vector3D kEcefDelta = p_ecef - kRefEcef;
  const Vector3D kNed = GlobalToNedRotation(ref_geo) * kEcefDelta;
  return LocalCoordinate{kNed.x_, kNed.y_, kNed.z_};
}

// =====================================================================
// Body ⇔ NED (LocalCoordinate)
// =====================================================================
//
// 機体座標系の軸 (x: 前方, y: 右, z: 下) と NED 軸 (N, E, D) は
// Z の向きが一致しているので、ZYX オイラー角の回転行列 R = Rz·Ry·Rx
// (FromAttitude が返すもの) がそのまま body → NED 変換になる。

/**
 * @brief BodyCoordinate (機体軸) を LocalCoordinate (NED) に変換する。
 *
 * 平行移動は伴わない (両者とも同一原点に置く規約)。回転のみを適用する。
 *
 * @param body     機体軸での位置 (= 重心からのオフセット) [m]。
 * @param attitude 機体姿勢 (ZYX オイラー角)。
 * @return NED 軸で表現した同じベクトル [m]。
 */
inline LocalCoordinate BodyToLocal(const BodyCoordinate& body, const Attitude& attitude) {
  const Matrix3x3 kR = FromAttitude(attitude);
  const Vector3D kBodyVec{body.X(), body.Y(), body.Z()};
  const Vector3D kNed = kR * kBodyVec;
  return LocalCoordinate{kNed.x_, kNed.y_, kNed.z_};
}

/**
 * @brief LocalCoordinate (NED) を BodyCoordinate (機体軸) に変換する。
 *
 * BodyToLocal の逆変換。回転行列の転置を用いる。
 *
 * @param local    NED 軸での位置 [m]。
 * @param attitude 機体姿勢 (ZYX オイラー角)。
 * @return 機体軸で表現した同じベクトル [m]。
 */
inline BodyCoordinate LocalToBody(const LocalCoordinate& local, const Attitude& attitude) {
  const Matrix3x3 kRt = FromAttitude(attitude).Transpose();
  const Vector3D kLocalVec{local.X(), local.Y(), local.Z()};
  const Vector3D kBody = kRt * kLocalVec;
  return BodyCoordinate{kBody.x_, kBody.y_, kBody.z_};
}

}  // namespace myapp::common

#endif  // MYAPP_COMMON_COORDINATE_CONVERSION_H
