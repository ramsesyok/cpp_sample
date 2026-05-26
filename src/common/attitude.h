#ifndef MYAPP_COMMON_ATTITUDE_H
#define MYAPP_COMMON_ATTITUDE_H

namespace myapp::common {

/**
 * @brief 姿勢角 (オイラー角)。
 *
 * phi_ (ロール), theta_ (ピッチ), psi_ (ヨー) の3成分を持つ。
 * 単位はラジアンを想定するが、本クラス自体は単位に依存しない。
 * 不変条件を持たないため struct とする。
 */
struct Attitude {
  double phi_;
  double theta_;
  double psi_;

  /** @brief 姿勢角の和 (this + rhs)。 */
  constexpr Attitude operator+(const Attitude& rhs) const {
    return {phi_ + rhs.phi_, theta_ + rhs.theta_, psi_ + rhs.psi_};
  }

  /** @brief 姿勢角の差 (this - rhs)。 */
  constexpr Attitude operator-(const Attitude& rhs) const {
    return {phi_ - rhs.phi_, theta_ - rhs.theta_, psi_ - rhs.psi_};
  }

  /** @brief スカラー倍 (this * s)。 */
  constexpr Attitude operator*(double s) const {
    return {phi_ * s, theta_ * s, psi_ * s};
  }

  /** @brief スカラー除算 (this / s)。 */
  constexpr Attitude operator/(double s) const {
    return {phi_ / s, theta_ / s, psi_ / s};
  }
};

/**
 * @brief スカラー左辺のスカラー倍 (s * a)。
 * @param s  スカラー。
 * @param a  姿勢角。
 * @return s 倍された姿勢角。
 */
constexpr Attitude operator*(double s, const Attitude& a) {
  return a * s;
}

}  // namespace myapp::common

#endif  // MYAPP_COMMON_ATTITUDE_H
