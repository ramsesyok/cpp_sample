#include "common/polygon.h"

namespace myapp::common {

bool contains(const Polygon& polygon, const Point& p) {
    if (polygon.size() < 3) {
        return false;
    }

    bool inside = false;
    const std::size_t n = polygon.size();
    for (std::size_t i = 0, j = n - 1; i < n; j = i++) {
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
    return inside;
}

}
