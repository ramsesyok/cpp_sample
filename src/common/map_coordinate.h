#ifndef MYAPP_COMMON_MAP_COORDINATE_H
#define MYAPP_COMMON_MAP_COORDINATE_H

namespace myapp::common {

/**
 * @brief 緯度経度高度 (度基準)。
 *
 * 地図表示・人間が読み書きする入出力で使う想定。ラジアン基準の
 * GeoCoordinate との取り違えを防ぐため独立した型とし、暗黙変換は
 * 持たせない (変換は coordinate_conversion.h の GeoToMap / MapToGeo を使う)。
 */
class MapCoordinate {
 public:
  /**
   * @brief 度基準の値からインスタンスを生成する。
   * @param latitude_deg   緯度 [deg]。
   * @param longitude_deg  経度 [deg]。
   * @param altitude_m     楕円体高 [m]。
   */
  MapCoordinate(double latitude_deg, double longitude_deg, double altitude_m)
      : lat_deg_{latitude_deg}, lon_deg_{longitude_deg}, alt_m_{altitude_m} {}

  /** @brief 緯度 [deg]。 */
  [[nodiscard]] double LatitudeDeg() const {
    return lat_deg_;
  }
  /** @brief 経度 [deg]。 */
  [[nodiscard]] double LongitudeDeg() const {
    return lon_deg_;
  }
  /** @brief 楕円体高 [m]。 */
  [[nodiscard]] double AltitudeM() const {
    return alt_m_;
  }

 private:
  double lat_deg_;
  double lon_deg_;
  double alt_m_;
};

}  // namespace myapp::common

#endif  // MYAPP_COMMON_MAP_COORDINATE_H
