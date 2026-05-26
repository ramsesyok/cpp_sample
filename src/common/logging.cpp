#include "common/logging.h"

#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>
#include <vector>

namespace myapp::common::logging {

namespace {

constexpr const char* kLoggerName = "myapp";
constexpr const char* kPattern = "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thr %t] %v";

}  // namespace

void Init(const Config& cfg) {
  // 非同期ロガーのバックエンド (スレッドプール) を初期化。
  // ※プロセス中で 1 度だけ呼ぶ。
  spdlog::init_thread_pool(cfg.async_queue_size_, cfg.async_thread_count_);

  // コンソール sink (色付き、stdout)。
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(cfg.console_level_);

  // ローテーティングファイル sink。
  auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
      cfg.log_file_path_, cfg.max_file_size_bytes_, cfg.max_files_);
  file_sink->set_level(cfg.file_level_);

  std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};

  // 非同期ロガーを構築。
  // overflow_policy = overrun_oldest がポイント:
  //   キューが満杯になっても呼び出しスレッドをブロックせず、
  //   古いメッセージを上書き破棄する。
  //   演算スレッドのレイテンシを最優先するならこれ一択。
  auto logger = std::make_shared<spdlog::async_logger>(
      kLoggerName, sinks.begin(), sinks.end(), spdlog::thread_pool(),
      spdlog::async_overflow_policy::overrun_oldest);

  // ロガー自体は全レベル通し、フィルタは sink 側に委ねる。
  logger->set_level(spdlog::level::trace);
  logger->set_pattern(kPattern);

  // warn 以上は即時 flush。
  // info/debug は async キュー任せで I/O コストを呼び出し側に乗せない。
  logger->flush_on(spdlog::level::warn);

  // 既定ロガーとして登録。これで SPDLOG_INFO(...) 等が
  // このロガー宛になる。
  spdlog::set_default_logger(logger);
}

void Shutdown() {
  // 残っているメッセージを flush し、スレッドプールを停止。
  spdlog::shutdown();
}

}  // namespace myapp::common::logging
