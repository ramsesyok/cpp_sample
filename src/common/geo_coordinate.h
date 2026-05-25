#pragma once

namespace myapp::common
{

    // 緯度経度高度 (ラジアン基準)。
    // 度単位の MapCoordinate との取り違えを防ぐため独立した型とし、
    // 暗黙変換は持たせない (変換は coordinate_conversion.h の to_map / to_geo を使う)。
    class GeoCoordinate
    {
    public:
        GeoCoordinate(double latitude_rad, double longitude_rad, double altitude_m)
            : lat_rad_{latitude_rad},
              lon_rad_{longitude_rad},
              alt_m_{altitude_m} {}

        double latitude() const { return lat_rad_; }
        double longitude() const { return lon_rad_; }
        double altitude() const { return alt_m_; }

    private:
        double lat_rad_;
        double lon_rad_;
        double alt_m_;
    };

} // namespace myapp::common
