// 例3: 地点 + 通過時刻の組み合わせで経路情報を作り、移動模擬を行う。
//
// Waypoint = {GlobalCoordinate, t (秒)}。
// 入力された複数の Waypoint の間を ECEF 直線で線形補間し、任意時刻 t の
// 位置と速度ベクトルを返すサンプル。
//
// 確認した型/オペレータの不足:
//   - Vector3D に Length() (ノルム) や Normalized() が無い。本サンプルで
//     局所ヘルパとして実装。
//   - GlobalCoordinate に「2点を比 r:(1-r) で内分」する補間関数が無い。
//     ここでは a + (b - a) * t と表現できるので問題はないが、
//     ベクトル化済みの Lerp があると直感的になる。
//   - GlobalCoordinate に operator+=(Vector3D) や operator-=(Vector3D)
//     が無い。逐次積算 (例: 等速度モデル) には += があると便利。

#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>

#include "common/coordinate_conversion.h"
#include "common/geo_coordinate.h"
#include "common/global_coordinate.h"
#include "common/map_coordinate.h"
#include "common/vector3d.h"

namespace mc = myapp::common;

namespace {

double Length(const mc::Vector3D& v) {
  return std::sqrt((v.x_ * v.x_) + (v.y_ * v.y_) + (v.z_ * v.z_));
}

struct Waypoint {
  mc::GlobalCoordinate position;
  double t_sec;
};

// 経路 [k番目, k+1番目] の区間で、時刻 t における ECEF 位置と速度を返す。
struct State {
  mc::GlobalCoordinate position;
  mc::Vector3D velocity;
};

State SampleAt(const std::vector<Waypoint>& path, double t_sec) {
  // 末端処理: 範囲外は端点に張り付ける (速度ゼロ)。
  if (t_sec <= path.front().t_sec) {
    return State{path.front().position, mc::Vector3D{0.0, 0.0, 0.0}};
  }
  if (t_sec >= path.back().t_sec) {
    return State{path.back().position, mc::Vector3D{0.0, 0.0, 0.0}};
  }
  for (std::size_t i = 1; i < path.size(); ++i) {
    if (t_sec <= path[i].t_sec) {
      const auto& a = path[i - 1];
      const auto& b = path[i];
      const double dt = b.t_sec - a.t_sec;
      const mc::Vector3D delta = b.position - a.position;
      const mc::Vector3D velocity = delta / dt;
      const double r = (t_sec - a.t_sec) / dt;
      // GlobalCoordinate + Vector3D を使った線形補間。
      const mc::GlobalCoordinate p = a.position + (delta * r);
      return State{p, velocity};
    }
  }
  // ここには到達しないはず。
  return State{path.back().position, mc::Vector3D{0.0, 0.0, 0.0}};
}

}  // namespace

int main() {
  // 経路を MapCoordinate で読み込み、ECEF に変換して保持する。
  const std::vector<std::pair<mc::MapCoordinate, double>> raw = {
      {{35.6812, 139.7671, 100.0}, 0.0},     // 東京付近、t=0s
      {{35.7000, 139.8000, 1000.0}, 60.0},   // 北東に上昇、t=60s
      {{35.7500, 139.8500, 1000.0}, 180.0},  // さらに北東に巡航、t=180s
      {{35.7700, 139.8800, 200.0}, 300.0},   // 降下、t=300s
  };

  std::vector<Waypoint> path;
  path.reserve(raw.size());
  for (const auto& [m, t] : raw) {
    path.push_back(Waypoint{mc::GeoToGlobal(mc::ToGeo(m)), t});
  }

  for (double t : {0.0, 30.0, 90.0, 150.0, 240.0, 300.0, 400.0}) {
    const State s = SampleAt(path, t);
    const mc::MapCoordinate m = mc::ToMap(mc::GlobalToGeo(s.position));
    std::cout << "t=" << t << "s  lat=" << m.LatitudeDeg()
              << " lon=" << m.LongitudeDeg() << " alt=" << m.AltitudeM()
              << "  |v|=" << Length(s.velocity) << " m/s\n";
  }

  return 0;
}
