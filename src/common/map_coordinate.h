#pragma once

namespace myapp::common {

// 緯度経度高度 (度基準)。地図表示・人間が読み書きする入出力で使う想定。
// ラジアン基準の GeoCoordinate との取り違えを防ぐため独立した型とし、
// 暗黙変換は持たせない (変換は coordinate_conversion.h の to_map / to_geo を使う)。
class MapCoordinate {
public:
    MapCoordinate(double latitude_deg, double longitude_deg, double altitude_m)
        : lat_deg_{latitude_deg},
          lon_deg_{longitude_deg},
          alt_m_{altitude_m} {}

    double latitude_deg() const { return lat_deg_; }
    double longitude_deg() const { return lon_deg_; }
    double altitude_m() const { return alt_m_; }

private:
    double lat_deg_;
    double lon_deg_;
    double alt_m_;
};

}  // namespace myapp::common
