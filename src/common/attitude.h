#ifndef MYAPP_COMMON_ATTITUDE_H
#define MYAPP_COMMON_ATTITUDE_H

namespace myapp::common {

/**
 * @brief 姿勢角 (オイラー角)。
 *
 * phi_ (ロール), theta_ (ピッチ), psi_ (ヨー) の3成分を持つ。
 * 単位はラジアンを想定するが、本クラス自体は単位に依存しない。
 * 不変条件を持たないため struct とする。
 *
 * シミュレーションでの積算用に複合代入演算子を提供する。なお、姿勢の
 * 時間積分にオイラー角を直接使うとジンバルロックの影響を受けるため、
 * 厳密な姿勢伝播にはクォータニオン経由を検討すること。
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

  /** @brief 単項マイナス (符号反転)。 */
  constexpr Attitude operator-() const {
    return {-phi_, -theta_, -psi_};
  }

  /** @brief スカラー倍 (this * s)。 */
  constexpr Attitude operator*(double s) const {
    return {phi_ * s, theta_ * s, psi_ * s};
  }

  /** @brief スカラー除算 (this / s)。 */
  constexpr Attitude operator/(double s) const {
    return {phi_ / s, theta_ / s, psi_ / s};
  }

  /** @brief 複合代入: 加算。 */
  constexpr Attitude& operator+=(const Attitude& rhs) {
    phi_ += rhs.phi_;
    theta_ += rhs.theta_;
    psi_ += rhs.psi_;
    return *this;
  }

  /** @brief 複合代入: 減算。 */
  constexpr Attitude& operator-=(const Attitude& rhs) {
    phi_ -= rhs.phi_;
    theta_ -= rhs.theta_;
    psi_ -= rhs.psi_;
    return *this;
  }

  /** @brief 複合代入: スカラー倍。 */
  constexpr Attitude& operator*=(double s) {
    phi_ *= s;
    theta_ *= s;
    psi_ *= s;
    return *this;
  }

  /** @brief 複合代入: スカラー除算。 */
  constexpr Attitude& operator/=(double s) {
    phi_ /= s;
    theta_ /= s;
    psi_ /= s;
    return *this;
  }

  /** @brief 厳密一致比較。 */
  constexpr bool operator==(const Attitude& rhs) const {
    return phi_ == rhs.phi_ && theta_ == rhs.theta_ && psi_ == rhs.psi_;
  }

  /** @brief 厳密一致比較の否定。 */
  constexpr bool operator!=(const Attitude& rhs) const {
    return !(*this == rhs);
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
