// 例2: A 地点から B 地点までの方位 (initial bearing) を算出する。
//
// MapCoordinate / GeoCoordinate を入力として、A から見た B の方位角
// (北を 0、東を +90 とする時計回り) を [0, 360) deg で返す。
//
// 確認した型/オペレータの不足:
//   - GeoCoordinate / MapCoordinate に「2点間の方位」「2点間の距離」
//     を求めるユーティリティが無い。本ファイル内に局所実装する。
//   - GeoCoordinate に operator==/!= が無い (本例では未使用だが、
//     等値比較したい場面は多そう)。
//
// 算式 (球面三角法 — WGS84 楕円体ではなく球面近似):
//   θ = atan2( sin(Δλ) cosφ2,
//              cosφ1 sinφ2 - sinφ1 cosφ2 cos(Δλ) )

#include <cmath>
#include <iostream>

#include "common/coordinate_conversion.h"
#include "common/geo_coordinate.h"
#include "common/map_coordinate.h"

namespace mc = myapp::common;

namespace {

// A から B への初期方位 (北を 0 とした deg, [0, 360))。
double InitialBearingDeg(const mc::GeoCoordinate& a, const mc::GeoCoordinate& b) {
  const double phi1 = a.Latitude();
  const double phi2 = b.Latitude();
  const double dlon = b.Longitude() - a.Longitude();

  const double y = std::sin(dlon) * std::cos(phi2);
  const double x = (std::cos(phi1) * std::sin(phi2)) -
                   (std::sin(phi1) * std::cos(phi2) * std::cos(dlon));
  constexpr double kRadToDeg = 57.29577951308232;
  const double bearing = std::atan2(y, x) * kRadToDeg;
  // [-180, 180] → [0, 360)。
  return std::fmod(bearing + 360.0, 360.0);
}

// 大円距離 [m] (Haversine, 平均地球半径)。
double GreatCircleDistanceM(const mc::GeoCoordinate& a, const mc::GeoCoordinate& b) {
  constexpr double kMeanEarthRadiusM = 6371008.8;
  const double phi1 = a.Latitude();
  const double phi2 = b.Latitude();
  const double dphi = phi2 - phi1;
  const double dlon = b.Longitude() - a.Longitude();
  const double s1 = std::sin(dphi * 0.5);
  const double s2 = std::sin(dlon * 0.5);
  const double h = (s1 * s1) + (std::cos(phi1) * std::cos(phi2) * s2 * s2);
  return 2.0 * kMeanEarthRadiusM * std::asin(std::sqrt(h));
}

}  // namespace

int main() {
  // 東京駅 → シドニー (約南南東)。
  const mc::GeoCoordinate tokyo = mc::ToGeo(mc::MapCoordinate{35.6812, 139.7671, 0.0});
  const mc::GeoCoordinate sydney = mc::ToGeo(mc::MapCoordinate{-33.8688, 151.2093, 0.0});

  std::cout << "Tokyo  -> Sydney: bearing = "
            << InitialBearingDeg(tokyo, sydney) << " deg, "
            << "distance = " << GreatCircleDistanceM(tokyo, sydney) << " m\n";

  // 真東のチェック: 同じ緯度で経度だけ +1 deg。
  const mc::GeoCoordinate p1 = mc::ToGeo(mc::MapCoordinate{0.0, 0.0, 0.0});
  const mc::GeoCoordinate p2 = mc::ToGeo(mc::MapCoordinate{0.0, 1.0, 0.0});
  std::cout << "Equator east step: bearing = "
            << InitialBearingDeg(p1, p2) << " deg (expected ~90)\n";

  // 真北のチェック: 同じ経度で緯度だけ +1 deg。
  const mc::GeoCoordinate p3 = mc::ToGeo(mc::MapCoordinate{1.0, 0.0, 0.0});
  std::cout << "Equator north step: bearing = "
            << InitialBearingDeg(p1, p3) << " deg (expected ~0)\n";

  return 0;
}
