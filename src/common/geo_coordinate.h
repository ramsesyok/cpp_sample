#ifndef MYAPP_COMMON_GEO_COORDINATE_H_
#define MYAPP_COMMON_GEO_COORDINATE_H_

namespace myapp::common {

/**
 * @brief 緯度経度高度 (ラジアン基準)。
 *
 * 度単位の MapCoordinate との取り違えを防ぐため独立した型とし、
 * 暗黙変換は持たせない (変換は coordinate_conversion.h の
 * ToMap / ToGeo を使う)。
 */
class GeoCoordinate {
 public:
  /**
   * @brief ラジアン基準の値からインスタンスを生成する。
   * @param latitude_rad   緯度 [rad]。
   * @param longitude_rad  経度 [rad]。
   * @param altitude_m     楕円体高 [m]。
   */
  GeoCoordinate(double latitude_rad, double longitude_rad, double altitude_m)
      : lat_rad_{latitude_rad},
        lon_rad_{longitude_rad},
        alt_m_{altitude_m} {}

  /** @brief 緯度 [rad]。 */
  double latitude() const { return lat_rad_; }
  /** @brief 経度 [rad]。 */
  double longitude() const { return lon_rad_; }
  /** @brief 楕円体高 [m]。 */
  double altitude() const { return alt_m_; }

 private:
  double lat_rad_;
  double lon_rad_;
  double alt_m_;
};

}  // namespace myapp::common

#endif  // MYAPP_COMMON_GEO_COORDINATE_H_
