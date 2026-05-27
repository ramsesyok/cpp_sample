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
    ${CMAKE_SOURCE_DIR}/tests/catch2/catch_amalgamated.cpp
)

# include パスを PUBLIC にしておくことで、
# catch2_main をリンクするテスト側で #include <catch2/catch_amalgamated.hpp>
# が解決できるようになる。
target_include_directories(catch2_main PUBLIC
    ${CMAKE_SOURCE_DIR}/tests
)

# --- テスト追加用のヘルパ関数 ---------------------------------------
# 同一パターンの add_executable / target_link_libraries / add_test を
# 各テストごとに 3 行書くのは冗長なので、ヘルパ関数に集約する。
#
# 使い方:
#   myapp_add_test(test_foo            test_foo.cpp PRIVATE_LIBS myapp_common)
#   myapp_add_test(test_greet          test_greet.cpp PRIVATE_LIBS myapp_lib)
#
# 引数:
#   name         : 実行ファイル名兼テスト名
#   sources      : ソースファイル (複数可)
#   PRIVATE_LIBS : 追加でリンクするライブラリ (catch2_main / coverage_flags は自動付与)
function(myapp_add_test name)
    cmake_parse_arguments(MAT "" "" "PRIVATE_LIBS" ${ARGN})
    # 残った位置引数をソースファイルとして扱う。
    set(sources ${MAT_UNPARSED_ARGUMENTS})

    add_executable(${name} ${sources})
    target_link_libraries(${name}
        PRIVATE
            ${MAT_PRIVATE_LIBS}
            catch2_main
            coverage_flags
    )
    add_test(NAME ${name} COMMAND ${name})
endfunction()

# --- tests 直下のテスト実行ファイル: test_greet ---------------------
# 1 つのテスト実行ファイル = 1 つのテスト対象モジュール、という方針。
# こうしておくと:
#   - リンクするライブラリが最小限になりビルド依存が明確
#   - CTest 上でモジュール単位の合否が見えて切り分けが楽
myapp_add_test(test_greet test_greet.cpp PRIVATE_LIBS myapp_lib)

# --- サブディレクトリのテストを取り込む ------------------------------
# tests/common/CMakeLists.txt を読み込み、test_polygon などを追加する。
add_subdirectory(common)
