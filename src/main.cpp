#include <exception>
#include <iostream>
#include <string>

#include "common/logging.h"
#include "greet.h"

int main(int argc, char* argv[]) {
  // main を抜けるときに自動でデストラクタが呼ばれることを利用して、
  // 確実に Shutdown を実行する。
  struct LoggingGuard {
    LoggingGuard() { myapp::common::logging::Init(); }
    ~LoggingGuard() { myapp::common::logging::Shutdown(); }
  } logging_guard;

  try {
    const std::string name = (argc > 1) ? argv[1] : "world";
    SPDLOG_INFO("myapp start: name={}", name);

    const auto message = myapp::Greet(name);
    std::cout << message << std::endl;

    SPDLOG_INFO("myapp finish");
    return 0;
  } catch (const std::exception& e) {
    // ここに来るのは想定外のエラーだけ。non-async 経路で確実に残す。
    SPDLOG_CRITICAL("unhandled exception: {}", e.what());
    return 1;
  }
}
