# =====================================================================
# src/common/CMakeLists.txt
#   アプリ内の複数モジュールから共通に使う処理を集めた
#   静的ライブラリ "myapp_common" を定義する。
# =====================================================================

# Threads は spdlog の非同期ロガーが必要とする (pthread 等)。
# REQUIRED にしておけば、見つからない環境では configure 時に
# エラーで止まり、リンク時の不可解な失敗を避けられる。
find_package(Threads REQUIRED)

# myapp_common 静的ライブラリ。
# ここに含めるのは「特定の業務ロジックに依存しない汎用処理」。
# 例: 幾何計算、文字列ユーティリティ、ロガー など。
add_library(myapp_common
    logging.cpp
)

# include パスの公開設定。
#   - ${CMAKE_SOURCE_DIR}/src を PUBLIC にすることで、myapp_common を
#     リンクする側は #include "common/vector3d.h" のように
#     "フォルダ名/ヘッダ名" の形で書ける。
#   - 3rdparty ヘッダ (spdlog, nlohmann/json 等) もここで通す。
target_include_directories(myapp_common
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/3rdparty/include
)

# spdlog はヘッダオンリーで使うので追加のリンク対象は無いが、
# 非同期ロガーがスレッドを使うので Threads は必須。
target_link_libraries(myapp_common
    PUBLIC
        Threads::Threads
    PRIVATE
        coverage_flags
)

# --- spdlog のコンパイル時設定 --------------------------------------
# SPDLOG_ACTIVE_LEVEL はソース全体に効かせたいので PUBLIC で公開する。
# こうすると、myapp_common をリンクする全ターゲット (myapp 本体やテスト)
# でも同じレベル設定が適用される。
#
# ジェネレータ式 $<CONFIG:...> でビルド構成ごとに切替:
#   Debug         -> TRACE (全レベル有効、開発時に詳細を見たい)
#   Debug 以外    -> INFO  (TRACE/DEBUG は呼び出しごと消滅し性能影響ゼロ)
target_compile_definitions(myapp_common
    PUBLIC
        $<$<CONFIG:Debug>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE>
        $<$<NOT:$<CONFIG:Debug>>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO>
)
