// 例1: グローバル座標で指定された位置にいるオブジェクトの中心から、
//      Vector3D 分オフセットした位置のグローバル座標を算出する。
//
// 注意点:
//   GlobalCoordinate には operator+(Vector3D) があるが、これは「ECEF 軸での
//   変位ベクトル」を加える操作である。一方、現場で扱いたい「オブジェクト
//   中心からのオフセット」は通常、機体座標系 (BodyCoordinate の軸)
//   または接平面ローカル座標系 (NEU) で与えられる。
//   したがって、そのまま GlobalCoordinate + Vector3D を呼ぶと意味が
//   一致しない。本サンプルではオフセットを次の経路で変換する:
//
//     BodyCoordinate のオフセット
//       --(姿勢 Attitude を回転行列にして適用)--> NEU 系のベクトル
//       --(参照点 lat/lon に基づく NEU→ECEF 回転)--> ECEF 系のベクトル
//       --(GlobalCoordinate + Vector3D)--> グローバル座標
//
// 確認した型/オペレータの不足:
//   - NEU (LocalCoordinate) と ECEF (GlobalCoordinate) の相互変換が
//     coordinate_conversion.h に未実装。本ファイル内で局所ヘルパとして実装。
//   - Vector3D に Length/Norm が無い (例3で再掲)。

#include <cmath>
#include <iostream>

#include "common/attitude.h"
#include "common/body_coordinate.h"
#include "common/coordinate_conversion.h"
#include "common/geo_coordinate.h"
#include "common/global_coordinate.h"
#include "common/map_coordinate.h"
#include "common/matrix3x3.h"
#include "common/vector3d.h"

namespace mc = myapp::common;

namespace {

// NEU (North-East-Up) 系を ECEF 系に向ける回転行列。
// 参照点の緯度経度 (rad) における接平面の各軸を ECEF 軸で表現する。
mc::Matrix3x3 NeuToEcefRotation(double lat_rad, double lon_rad) {
  const double s_lat = std::sin(lat_rad);
  const double c_lat = std::cos(lat_rad);
  const double s_lon = std::sin(lon_rad);
  const double c_lon = std::cos(lon_rad);
  // 列ベクトルが順に North, East, Up を表す。
  return mc::Matrix3x3{{{
      {{-s_lat * c_lon, -s_lon, c_lat * c_lon}},
      {{-s_lat * s_lon, c_lon, c_lat * s_lon}},
      {{c_lat, 0.0, s_lat}},
  }}};
}

}  // namespace

int main() {
  // 機体は東京駅付近の上空 500m。
  const mc::MapCoordinate origin_map{35.6812, 139.7671, 500.0};
  const mc::GeoCoordinate origin_geo = mc::ToGeo(origin_map);
  const mc::GlobalCoordinate origin_ecef = mc::GeoToGlobal(origin_geo);

  // 姿勢: ヨー 90deg (機体は東を向く)、ピッチ・ロール 0。
  constexpr double kDegToRad = 0.017453292519943295;
  const mc::Attitude att{0.0, 0.0, 90.0 * kDegToRad};

  // 機体座標系での点 (前方 5m、右方 0、下方 -2m = 機体重心の上方 2m)。
  const mc::BodyCoordinate sensor_body{5.0, 0.0, -2.0};

  // BodyCoordinate を「機体原点からのオフセットベクトル」として取り出す。
  // BodyCoordinate には ToVector() のような明示変換が無いため、
  // 「原点との差」で Vector3D 化する。
  const mc::Vector3D offset_body = sensor_body - mc::BodyCoordinate{0.0, 0.0, 0.0};

  // Body 軸 (前方 x, 右方 y, 下方 z) を NEU (北 x, 東 y, 上 z) に直す。
  // matrix3x3.h の FromAttitude は ZYX オイラー角 (body → NEU) を返す
  // が、軸の取り方が body=(F,R,D)、NEU=(N,E,U) と異なるため、軸並べ替え
  // 行列 P を挟む必要がある:
  //   v_neu = R(att) * P * v_body
  //   P は body の (F,R,D) を NEU の (F,R,-D) に並べ替える単純な符号付け替え。
  const mc::Matrix3x3 perm{{{
      {{1.0, 0.0, 0.0}},
      {{0.0, 1.0, 0.0}},
      {{0.0, 0.0, -1.0}},
  }}};
  const mc::Matrix3x3 r_body_to_neu = mc::FromAttitude(att);
  const mc::Vector3D offset_neu = r_body_to_neu * (perm * offset_body);

  // NEU → ECEF。
  const mc::Matrix3x3 r_neu_to_ecef =
      NeuToEcefRotation(origin_geo.Latitude(), origin_geo.Longitude());
  const mc::Vector3D offset_ecef = r_neu_to_ecef * offset_neu;

  // GlobalCoordinate + Vector3D で最終位置。
  const mc::GlobalCoordinate sensor_ecef = origin_ecef + offset_ecef;
  const mc::GeoCoordinate sensor_geo = mc::GlobalToGeo(sensor_ecef);
  const mc::MapCoordinate sensor_map = mc::ToMap(sensor_geo);

  std::cout << "origin (map): " << origin_map.LatitudeDeg() << ", "
            << origin_map.LongitudeDeg() << ", alt=" << origin_map.AltitudeM() << "\n";
  std::cout << "sensor (map): " << sensor_map.LatitudeDeg() << ", "
            << sensor_map.LongitudeDeg() << ", alt=" << sensor_map.AltitudeM() << "\n";

  // 期待: ヨー 90deg なので機体の前方 (Body+X) は東方向に伸びる。
  //       オフセットの上方 2m 分だけ alt が上昇する。
  return 0;
}
