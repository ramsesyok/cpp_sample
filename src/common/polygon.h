#pragma once

#include <vector>

namespace myapp::common {

struct Point {
    double x;
    double y;
};

using Polygon = std::vector<Point>;

// Ray-casting による多角形に対する点の内外判定。
// 辺上の点の扱いは未定義（サンプル用途のため厳密化していない）。
bool contains(const Polygon& polygon, const Point& p);

}
