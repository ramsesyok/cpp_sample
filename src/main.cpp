#include <exception>
#include <iostream>
#include <string>

#include "common/logging.h"
#include "greet.h"

int main(int argc, char* argv[]) {
  try {
    // main を抜けるときに自動でデストラクタが呼ばれることを利用して、
    // 確実に Shutdown を実行する。
    struct LoggingGuard {
      LoggingGuard() {
        myapp::common::logging::Init();
      }
      ~LoggingGuard() {
        myapp::common::logging::Shutdown();
      }
      LoggingGuard(const LoggingGuard&) = delete;
      LoggingGuard& operator=(const LoggingGuard&) = delete;
      LoggingGuard(LoggingGuard&&) = delete;
      LoggingGuard& operator=(LoggingGuard&&) = delete;
    } logging_guard;

    const std::string kName = (argc > 1) ? argv[1] : "world";
    SPDLOG_INFO("myapp start: name={}", kName);

    const auto kMessage = myapp::Greet(kName);
    std::cout << kMessage << '\n';

    SPDLOG_INFO("myapp finish");
    return 0;
  } catch (const std::exception& e) {
    // ここに来るのは想定外のエラーだけ。non-async 経路で確実に残す。
    // SPDLOG_CRITICAL 自体が投げる可能性があるので内側でも捕捉する。
    try {
      SPDLOG_CRITICAL("unhandled exception: {}", e.what());
    } catch (...) {  // NOLINT(bugprone-empty-catch)
      // ロガーが死んでいたら諦める。main から例外を出さないことを優先する。
    }
    return 1;
  } catch (...) {
    // 非 std::exception 派生も含めて main からの脱出を完全に塞ぐ。
    return 1;
  }
}
