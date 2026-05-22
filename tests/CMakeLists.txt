# =====================================================================
# tests/CMakeLists.txt
#   Catch2 を使った単体テストの土台と、tests 直下のテスト実行ファイル
#   を定義する。サブディレクトリ (common 等) ごとのテストは
#   それぞれの CMakeLists.txt 側で追加する。
# =====================================================================

# --- Catch2 を共有ライブラリ化 --------------------------------------
# Catch2 v3 の amalgamated 版 (catch_amalgamated.cpp) は約 2 万行あり
# 毎回コンパイルすると重い。
# そこで一度だけ静的ライブラリとしてビルドし、各テスト実行ファイルから
# リンクすることで、ビルド時間を大幅に短縮する。
add_library(catch2_main STATIC
    ${CMAKE_SOURCE_DIR}/3rdparty/include/catch2/catch_amalgamated.cpp
)

# include パスを PUBLIC にしておくことで、
# catch2_main をリンクするテスト側で #include <catch2/catch_amalgamated.hpp>
# が解決できるようになる。
target_include_directories(catch2_main PUBLIC
    ${CMAKE_SOURCE_DIR}/3rdparty/include
)

# --- tests 直下のテスト実行ファイル: test_greet ---------------------
# 1 つのテスト実行ファイル = 1 つのテスト対象モジュール、という方針。
# こうしておくと:
#   - リンクするライブラリが最小限になりビルド依存が明確
#   - CTest 上でモジュール単位の合否が見えて切り分けが楽
add_executable(test_greet test_greet.cpp)

# テスト対象のライブラリ (myapp_lib) と Catch2 本体 (catch2_main) をリンク。
# 用途的にこの実行ファイル内でしか使わないので PRIVATE で良い。
target_link_libraries(test_greet PRIVATE myapp_lib catch2_main)

# CTest に登録する。
#   - NAME: ctest が表示・選択に使う名前
#   - COMMAND: 実際に起動するプログラム (ここでは同名のターゲット)
# 登録すると `ctest` コマンドや `ctest -R test_greet` で実行できる。
add_test(NAME test_greet COMMAND test_greet)

# --- サブディレクトリのテストを取り込む ------------------------------
# tests/common/CMakeLists.txt を読み込み、test_polygon などを追加する。
add_subdirectory(common)
