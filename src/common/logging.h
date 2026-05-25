#ifndef MYAPP_COMMON_LOGGING_H_
#define MYAPP_COMMON_LOGGING_H_

// ============================================================
// SPDLOG_ACTIVE_LEVEL はビルド構成ごとに CMake から定義する。
//   - Release : SPDLOG_LEVEL_INFO  (TRACE/DEBUG はコードごと消える)
//   - Debug等 : SPDLOG_LEVEL_TRACE (全レベル有効)
// ここでは念のためフォールバックを定義しておく。
// ============================================================
#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#endif

#include <spdlog/spdlog.h>

#include <cstddef>
#include <string>

namespace myapp::common::logging {

/**
 * @brief ロギング初期化用の設定値。
 *
 * 既定値で実用上の挙動になるよう調整済み。必要に応じて個別に
 * 上書きしてから Init() に渡す。
 */
struct Config {
  /** @brief ファイル出力先 (ディレクトリは存在していなくても spdlog が作る)。 */
  std::string log_file_path = "logs/myapp.log";

  /**
   * @brief コンソール sink のランタイムレベル。
   *
   * コンパイル時に SPDLOG_ACTIVE_LEVEL より低いレベルは
   * どのみち消えるため、ここで TRACE にしても無意味なことに注意。
   */
  spdlog::level::level_enum console_level = spdlog::level::info;
  /** @brief ファイル sink のランタイムレベル。 */
  spdlog::level::level_enum file_level = spdlog::level::debug;

  /** @brief 非同期キューの長さ (メッセージ件数、2 のべき乗推奨)。 */
  std::size_t async_queue_size = 8192;
  /** @brief バックグラウンド flush スレッド数。 */
  std::size_t async_thread_count = 1;

  /** @brief ローテーティングファイル sink の 1 ファイル最大サイズ [bytes]。 */
  std::size_t max_file_size_bytes = 5 * 1024 * 1024;  // 5 MiB
  /** @brief 残す世代数。 */
  std::size_t max_files = 3;
};

/**
 * @brief ロギングサブシステムを初期化する。
 * @param cfg  ロギング設定 (省略時は既定値)。
 *
 * プロセス起動直後に 1 度だけ呼ぶ。再入不可。
 */
void Init(const Config& cfg = {});

/**
 * @brief ロギングサブシステムを停止する。
 *
 * プロセス終了直前に呼び、非同期キューを flush し切る。
 * 呼ばないとプロセス終了時にメッセージが落ちる可能性がある。
 */
void Shutdown();

}  // namespace myapp::common::logging

// ============================================================
// 利用側へのルール:
//   ログは必ず SPDLOG_xxx マクロ経由で出す (logger->info(...) 等は使わない)。
//   マクロは SPDLOG_ACTIVE_LEVEL によりプリプロセッサで消えるため、
//   無効化されたレベルでは "引数の評価コストすら 0" になる。
//
//   例:
//     SPDLOG_TRACE("inside loop: i={}, value={}", i, expensive_calc());
//   この expensive_calc() は Release ビルドでは呼ばれもしない。
// ============================================================

#endif  // MYAPP_COMMON_LOGGING_H_
