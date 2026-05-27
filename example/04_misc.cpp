// 例4: その他、座標を用いた典型計算のサンプル。
//   (a) 2 つの GlobalCoordinate 間の直線距離 (ECEF chord 距離) と
//       NED 系で見た北/東/下成分への分解。
//   (b) Vector3D の外積 (Cross product) を使った面法線の計算。
//   (c) 姿勢角 (Attitude) の補間 (オイラー角の単純線形補間)。
//   (d) BodyCoordinate を World に投影してみる逆方向の動作。
//
// 確認した型/オペレータの不足 (具体例は findings.md にまとめる):
//   - Vector3D に Cross() が無い。ここで局所実装。
//   - Vector3D に operator-（単項マイナス）が無い。反転を多用する場面で
//     0 - v と書くのは冗長。
//   - Vector3D に operator==/!= が無い (テストや等値判定で欲しい)。
//   - Attitude も Vector3D も += /-= /*= /\=  の複合代入が無い。
//   - LocalCoordinate <-> GlobalCoordinate のフレーム変換が未提供。
//     ここでは例1と同じヘルパを再掲。

#include <cmath>
#include <iostream>

#include "common/attitude.h"
#include "common/body_coordinate.h"
#include "common/coordinate_conversion.h"
#include "common/geo_coordinate.h"
#include "common/global_coordinate.h"
#include "common/local_coordinate.h"
#include "common/map_coordinate.h"
#include "common/matrix3x3.h"
#include "common/vector3d.h"

namespace mc = myapp::common;

namespace {

mc::Vector3D Cross(const mc::Vector3D& a, const mc::Vector3D& b) {
  return mc::Vector3D{(a.y_ * b.z_) - (a.z_ * b.y_),
                      (a.z_ * b.x_) - (a.x_ * b.z_),
                      (a.x_ * b.y_) - (a.y_ * b.x_)};
}

double Length(const mc::Vector3D& v) {
  return std::sqrt(v.Dot(v));
}

// ECEF → NED の回転 (例1の転置に相当)。
mc::Matrix3x3 EcefToNedRotation(double lat_rad, double lon_rad) {
  const double s_lat = std::sin(lat_rad);
  const double c_lat = std::cos(lat_rad);
  const double s_lon = std::sin(lon_rad);
  const double c_lon = std::cos(lon_rad);
  return mc::Matrix3x3{
      -s_lat * c_lon, -s_lat * s_lon, c_lat,
      -s_lon,         c_lon,          0.0,
      -c_lat * c_lon, -c_lat * s_lon, -s_lat,
  };
}

}  // namespace

int main() {
  // (a) 2 点間距離と NED 分解。
  const mc::GeoCoordinate a_geo = mc::ToGeo(mc::MapCoordinate{35.6812, 139.7671, 0.0});
  const mc::GeoCoordinate b_geo = mc::ToGeo(mc::MapCoordinate{35.6900, 139.7800, 50.0});
  const mc::GlobalCoordinate a_ecef = mc::GeoToGlobal(a_geo);
  const mc::GlobalCoordinate b_ecef = mc::GeoToGlobal(b_geo);

  const mc::Vector3D ecef_delta = b_ecef - a_ecef;
  std::cout << "chord distance = " << Length(ecef_delta) << " m\n";

  const mc::Matrix3x3 r = EcefToNedRotation(a_geo.Latitude(), a_geo.Longitude());
  const mc::Vector3D ned_delta = r * ecef_delta;
  std::cout << "NED delta: N=" << ned_delta.x_ << " E=" << ned_delta.y_
            << " D=" << ned_delta.z_ << "\n";

  // (b) 外積で 3 点が作る平面の法線。
  const mc::Vector3D p_ab = b_ecef - a_ecef;
  const mc::GeoCoordinate c_geo = mc::ToGeo(mc::MapCoordinate{35.6700, 139.7900, 0.0});
  const mc::GlobalCoordinate c_ecef = mc::GeoToGlobal(c_geo);
  const mc::Vector3D p_ac = c_ecef - a_ecef;
  const mc::Vector3D normal = Cross(p_ab, p_ac);
  std::cout << "plane normal (unnormalized): ("
            << normal.x_ << ", " << normal.y_ << ", " << normal.z_ << ")\n";

  // (c) Attitude の線形補間 (短時間ステップでの単純な平均など)。
  const mc::Attitude a0{0.0, 0.0, 0.0};
  const mc::Attitude a1{0.1, 0.2, 0.3};
  // Attitude には Lerp 関数も += も無いが、+ - * があるので式で書ける。
  const double s = 0.4;
  const mc::Attitude blended = a0 + ((a1 - a0) * s);
  std::cout << "attitude blend (s=0.4): phi=" << blended.phi_
            << " theta=" << blended.theta_ << " psi=" << blended.psi_ << "\n";

  // (d) Body→World: BodyCoordinate のオフセットを姿勢ゼロ・参照点 a で
  //     ローカル系に変換し、さらに NED→ECEF。
  const mc::BodyCoordinate offset_b{10.0, 0.0, 0.0};  // 前方 10m
  const mc::Vector3D ob_vec = offset_b - mc::BodyCoordinate{0.0, 0.0, 0.0};
  // 軸の対応 (F,R,D) → (N,E,D) は同じ符号。姿勢ゼロなので回転も恒等。
  const mc::Vector3D ob_ned = ob_vec;
  const mc::Matrix3x3 r_n2e = r.Transpose();
  const mc::Vector3D ob_ecef = r_n2e * ob_ned;
  const mc::GlobalCoordinate target = a_ecef + ob_ecef;
  const mc::MapCoordinate target_map = mc::ToMap(mc::GlobalToGeo(target));
  std::cout << "10m forward from A: lat=" << target_map.LatitudeDeg()
            << " lon=" << target_map.LongitudeDeg()
            << " alt=" << target_map.AltitudeM() << "\n";

  return 0;
}
