#ifndef MYAPP_COMMON_VECTOR_2D_H
#define MYAPP_COMMON_VECTOR_2D_H

#include <cmath>

namespace myapp::common {

/**
 * @brief 2次元ベクトル。
 *
 * x_, y_ の独立した2成分を持ち、四則演算 (スカラー演算含む) と
 * 内積・外積 (スカラー値)・ノルム関連を提供する。不変条件を持たないため struct とする。
 *
 * 数値シミュレーションのループでは状態の積算 (`v += a * dt` 等) が頻出する
 * ため、複合代入 (`+=`, `-=`, `*=`, `/=`) と単項マイナスも提供する。
 *
 * 注意: 本型はフレーム情報を持たない。座標系の取り違えによるバグは型では
 * 防げないため、利用側のコーディング規約で吸収する。
 */
struct Vector2D {
  double x_;
  double y_;

  /** @brief ベクトル和 (this + rhs)。 */
  constexpr Vector2D operator+(const Vector2D& rhs) const {
    return {x_ + rhs.x_, y_ + rhs.y_};
  }

  /** @brief ベクトル差 (this - rhs)。 */
  constexpr Vector2D operator-(const Vector2D& rhs) const {
    return {x_ - rhs.x_, y_ - rhs.y_};
  }

  /** @brief 単項マイナス (符号反転)。 */
  constexpr Vector2D operator-() const {
    return {-x_, -y_};
  }

  /** @brief スカラー倍 (this * s)。 */
  constexpr Vector2D operator*(double s) const {
    return {x_ * s, y_ * s};
  }

  /** @brief スカラー除算 (this / s)。 */
  constexpr Vector2D operator/(double s) const {
    return {x_ / s, y_ / s};
  }

  /** @brief 複合代入: 加算。 */
  constexpr Vector2D& operator+=(const Vector2D& rhs) {
    x_ += rhs.x_;
    y_ += rhs.y_;
    return *this;
  }

  /** @brief 複合代入: 減算。 */
  constexpr Vector2D& operator-=(const Vector2D& rhs) {
    x_ -= rhs.x_;
    y_ -= rhs.y_;
    return *this;
  }

  /** @brief 複合代入: スカラー倍。 */
  constexpr Vector2D& operator*=(double s) {
    x_ *= s;
    y_ *= s;
    return *this;
  }

  /** @brief 複合代入: スカラー除算。 */
  constexpr Vector2D& operator/=(double s) {
    x_ /= s;
    y_ /= s;
    return *this;
  }

  /** @brief 厳密一致比較 (浮動小数の bit-for-bit 比較)。許容誤差付き比較は別途必要。 */
  constexpr bool operator==(const Vector2D& rhs) const {
    return x_ == rhs.x_ && y_ == rhs.y_;
  }

  /** @brief 厳密一致比較の否定。 */
  constexpr bool operator!=(const Vector2D& rhs) const {
    return !(*this == rhs);
  }

  /**
   * @brief 内積を返す。
   * @param rhs  相手側ベクトル。
   * @return 内積値 x·x' + y·y'。
   */
  [[nodiscard]] constexpr double Dot(const Vector2D& rhs) const {
    return (x_ * rhs.x_) + (y_ * rhs.y_);
  }

  /**
   * @brief 2次元外積をスカラー値で返す (this × rhs)。
   *
   * 2次元では外積は z 成分のみのスカラー値となる。
   * 符号は右手系の慣例に従い、正なら rhs が this から見て反時計回り側にある。
   * @param rhs  相手側ベクトル。
   * @return 外積のスカラー値 x·y' - y·x'。
   */
  [[nodiscard]] constexpr double Cross(const Vector2D& rhs) const {
    return (x_ * rhs.y_) - (y_ * rhs.x_);
  }

  /** @brief ノルム二乗 (= Dot(self))。平方根なしで距離比較したいときに使う。 */
  [[nodiscard]] constexpr double SquaredNorm() const {
    return (x_ * x_) + (y_ * y_);
  }

  /** @brief ノルム (ユークリッド長)。 */
  [[nodiscard]] double Norm() const {
    return std::sqrt(SquaredNorm());
  }

  /**
   * @brief 単位ベクトル化したコピーを返す。
   *
   * ノルムが 0 のベクトルに対しては (0, 0) を返す (NaN を返さない)。
   * 呼び出し側がゼロベクトルを扱わない前提なら、Norm() を直接見て自分で
   * チェックすることを推奨する。
   */
  [[nodiscard]] Vector2D Normalized() const {
    const double kN = Norm();
    if (kN == 0.0) {
      return Vector2D{0.0, 0.0};
    }
    return Vector2D{x_ / kN, y_ / kN};
  }
};

/**
 * @brief スカラー左辺のスカラー倍 (s * v)。
 * @param s  スカラー。
 * @param v  ベクトル。
 * @return s 倍されたベクトル。
 */
constexpr Vector2D operator*(double s, const Vector2D& v) {
  return v * s;
}

/**
 * @brief 内積の自由関数版 (numpy.dot 風)。
 *
 * `a.Dot(b)` と等価だが、`Dot(a, b)` の方が左右対称で読みやすい場面がある。
 * @param a  左辺ベクトル。
 * @param b  右辺ベクトル。
 * @return 内積値。
 */
[[nodiscard]] constexpr double Dot(const Vector2D& a, const Vector2D& b) {
  return a.Dot(b);
}

}  // namespace myapp::common

#endif  // MYAPP_COMMON_VECTOR_2D_H
