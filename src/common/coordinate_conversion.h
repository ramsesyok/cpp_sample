#pragma once

#include <cmath>

#include "common/ecef_position.h"
#include "common/geo_coordinate.h"
#include "common/map_coordinate.h"

namespace myapp::common
{

    // 座標系間の明示的な変換関数群。
    // 暗黙変換を許さないことが設計上の意図なので、変換は常にこの関数経由で明示する。
    //
    // 楕円体は WGS84 を使用する。
    // ecef -> geo は閉形式 (Bowring 1976) を採用。地表近傍 (~航空機高度) で
    // 数値誤差レベルの精度が反復なしで得られる。極や軌道高度を多用する場合は
    // Heikkinen の閉形式に差し替えるとよい。

    // WGS84 楕円体パラメータ。
    //   a : 長半径 [m]
    //   f : 扁平率 (1/298.257223563)
    //   e2: 第一離心率の二乗 = 2f - f^2
    namespace wgs84
    {
        inline constexpr double kSemiMajorAxisM = 6378137.0;
        inline constexpr double kFlattening = 1.0 / 298.257223563;
        inline constexpr double kFirstEccentricitySq =
            kFlattening * (2.0 - kFlattening);
    } // namespace wgs84

    inline MapCoordinate to_map(const GeoCoordinate &g)
    {
        // 180 / pi。pi は M_PI が処理系依存なので定数を直書きする。
        constexpr double kRadToDeg = 57.29577951308232087679815481410517;
        return MapCoordinate{
            g.latitude() * kRadToDeg,
            g.longitude() * kRadToDeg,
            g.altitude(),
        };
    }

    inline GeoCoordinate to_geo(const MapCoordinate &m)
    {
        // pi / 180。
        constexpr double kDegToRad = 0.01745329251994329576923690768488613;
        return GeoCoordinate{
            m.latitude_deg() * kDegToRad,
            m.longitude_deg() * kDegToRad,
            m.altitude_m(),
        };
    }

    // WGS84 楕円体上のジオデティック座標 (φ, λ, h) を ECEF (X, Y, Z) [m] に変換する。
    //
    //   N(φ) = a / sqrt(1 - e^2 sin^2 φ)        ※ 卯酉線曲率半径
    //   X = (N + h) cos φ cos λ
    //   Y = (N + h) cos φ sin λ
    //   Z = (N (1 - e^2) + h) sin φ
    //
    // 入力は GeoCoordinate (ラジアン基準) なので、度入力したい場合は
    // to_geo() で一度変換してから渡すこと。型で取り違えを防ぐ意図。
    inline EcefPosition geo_to_ecef(const GeoCoordinate &g)
    {
        const double sin_lat = std::sin(g.latitude());
        const double cos_lat = std::cos(g.latitude());
        const double sin_lon = std::sin(g.longitude());
        const double cos_lon = std::cos(g.longitude());

        const double n = wgs84::kSemiMajorAxisM /
                         std::sqrt(1.0 - wgs84::kFirstEccentricitySq * sin_lat * sin_lat);
        const double n_plus_h = n + g.altitude();

        const double x = n_plus_h * cos_lat * cos_lon;
        const double y = n_plus_h * cos_lat * sin_lon;
        const double z = (n * (1.0 - wgs84::kFirstEccentricitySq) + g.altitude()) * sin_lat;

        return EcefPosition{x, y, z};
    }

    // ECEF (X, Y, Z) [m] を WGS84 ジオデティック座標 (φ, λ, h) に変換する (Bowring 1976)。
    //
    // 経度は λ = atan2(Y, X) で常に厳密。緯度 φ は補助緯度 β を経由して閉形式で求める:
    //   tan β = (Z · a) / (p · b)              ※ p = sqrt(X² + Y²)
    //   tan φ = (Z + e'² · b · sin³β) / (p - e² · a · cos³β)
    // 高度は h = p / cosφ - N で求めるが、極近傍では cosφ ≈ 0 になるため
    // 代替式 h = |Z| / |sinφ| - N(1 - e²) に切り替える。
    inline GeoCoordinate ecef_to_geo(const EcefPosition &p_ecef)
    {
        constexpr double a = wgs84::kSemiMajorAxisM;
        constexpr double f = wgs84::kFlattening;
        constexpr double e2 = wgs84::kFirstEccentricitySq;
        constexpr double b = a * (1.0 - f);          // 短半径
        constexpr double ep2 = e2 / (1.0 - e2);      // 第二離心率の二乗 e'²
        constexpr double kHalfPi = 1.5707963267948966;

        const double x = p_ecef.x();
        const double y = p_ecef.y();
        const double z = p_ecef.z();

        const double p = std::sqrt(x * x + y * y);
        const double lon = std::atan2(y, x);

        // 自転軸上 (p ≈ 0): 経度は不定なので 0 を返し、緯度は ±π/2、
        // 高度は Z 軸上での原点からの距離から短半径 b を引いた値。
        if (p == 0.0)
        {
            const double lat = (z >= 0.0) ? kHalfPi : -kHalfPi;
            const double h = std::abs(z) - b;
            return GeoCoordinate{lat, 0.0, h};
        }

        // 補助緯度 β の初期推定。
        const double beta = std::atan2(z * a, p * b);
        const double sin_beta = std::sin(beta);
        const double cos_beta = std::cos(beta);

        // Bowring の閉形式による緯度 φ。
        // atan2 を使うことで分母の符号反転 (赤道直下や逆方向) でも正しく場合分けされる。
        const double lat = std::atan2(
            z + ep2 * b * sin_beta * sin_beta * sin_beta,
            p - e2 * a * cos_beta * cos_beta * cos_beta);

        const double sin_lat = std::sin(lat);
        const double cos_lat = std::cos(lat);
        const double n = a / std::sqrt(1.0 - e2 * sin_lat * sin_lat);

        // 高度。極近傍では p/cosφ が発散するため、Z 軸方向の式に切り替える。
        // しきい値 1e-10 はラジアンで約 90° から ~6×10⁻⁹° に相当する小角。
        double h;
        if (std::abs(cos_lat) > 1e-10)
        {
            h = p / cos_lat - n;
        }
        else
        {
            h = std::abs(z) / std::abs(sin_lat) - n * (1.0 - e2);
        }

        return GeoCoordinate{lat, lon, h};
    }

} // namespace myapp::common
