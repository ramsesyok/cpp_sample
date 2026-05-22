#include "common/polygon.h"

#include "common/logging.h"

namespace myapp::common {

bool contains(const Polygon& polygon, const Point& p) {
    // 関数境界 (= 低頻度) でのみログを出す。
    // SPDLOG_TRACE は Release では呼び出しごと消えるため、
    // 引数 polygon.size() の参照すら発生しない。
    SPDLOG_TRACE("contains(): vertices={}, point=({}, {})",
                 polygon.size(), p.x, p.y);

    if (polygon.size() < 3) {
        SPDLOG_DEBUG("contains(): degenerate polygon, returning false");
        return false;
    }

    bool inside = false;
    const std::size_t n = polygon.size();
    for (std::size_t i = 0, j = n - 1; i < n; j = i++) {
        // ※ ホットループ内では原則ログを書かない。
        //   どうしても入れる場合も SPDLOG_TRACE に留め、
        //   Release では完全に消える前提で使う。
        const Point& a = polygon[i];
        const Point& b = polygon[j];

        const bool straddles = (a.y > p.y) != (b.y > p.y);
        if (!straddles) {
            continue;
        }
        const double x_intersect = (b.x - a.x) * (p.y - a.y) / (b.y - a.y) + a.x;
        if (p.x < x_intersect) {
            inside = !inside;
        }
    }

    SPDLOG_TRACE("contains(): result={}", inside);
    return inside;
}

}  // namespace myapp::common
