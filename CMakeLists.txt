# =====================================================================
# ルート CMakeLists.txt
#   プロジェクト全体の入口となるファイル。
#   `cmake -S . -B build` を実行したとき最初に読まれる。
# =====================================================================

# 使用する CMake の最低バージョン。
# 3.16 は Visual Studio 2019 / Ubuntu 20.04 などに標準搭載されており、
# 現代的な機能 (target_* 系コマンド等) を一通り使える安心ライン。
cmake_minimum_required(VERSION 3.16)

# プロジェクト名と使用言語を宣言する。
#   - プロジェクト名 "myapp" は ${PROJECT_NAME} 変数として参照可能。
#   - CXX = C++ を使う、という指定。C を使わないなら CXX だけで良い。
project(myapp CXX)

# --- C++ の言語設定 -------------------------------------------------
# 使用する C++ 標準のバージョン (ここでは C++17)。
set(CMAKE_CXX_STANDARD 17)
# ON にすると「C++17 が使えないコンパイラならエラー」になる。
# OFF (既定) だと黙って古い標準に落とされる可能性があり危険。
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# OFF にすると GNU 拡張 (-std=gnu++17 等) を使わず、純粋な ISO C++ になる。
# 移植性を高めたいので OFF が無難。
set(CMAKE_CXX_EXTENSIONS OFF)

# --- テストの有効化 -------------------------------------------------
# CTest を有効にするおまじない。
# これを書いておくと、後で各 CMakeLists.txt で add_test() したものが
# `ctest` コマンドから一覧・実行できるようになる。
# ※ ルート側で 1 回だけ呼ぶのが鉄則。
enable_testing()

# --- gcov カバレッジ計装 (GCC/Clang, Linux 専用) --------------------
# 使い方:
#   cmake -S . -B build -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
#   cmake --build build
#   ctest --test-dir build        # .gcda ファイルを生成
#   cmake --build build --target coverage   # レポート生成 (gcovr が必要)
option(ENABLE_COVERAGE "gcov によるカバレッジ計装を有効にする" OFF)

# INTERFACE ライブラリとして常に作成しておく。
# OFF 時はフラグなしの空ターゲットになるため、
# サブディレクトリ側で条件分岐なしに target_link_libraries で参照できる。
add_library(coverage_flags INTERFACE)
if(ENABLE_COVERAGE)
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(WARNING
            "ENABLE_COVERAGE=ON ですが CMAKE_BUILD_TYPE が Debug ではありません。"
            " 最適化によりカバレッジ結果が不正確になる可能性があります。"
            " -DCMAKE_BUILD_TYPE=Debug の併用を推奨します。")
    endif()
    target_compile_options(coverage_flags INTERFACE --coverage)
    target_link_options(coverage_flags INTERFACE --coverage)

    # gcovr が見つかれば HTML レポートを生成する `coverage` ターゲットを追加。
    find_program(GCOVR_EXECUTABLE gcovr)
    if(GCOVR_EXECUTABLE)
        add_custom_target(coverage
            COMMAND ${GCOVR_EXECUTABLE}
                --root ${CMAKE_SOURCE_DIR}/src
                --exclude ${CMAKE_SOURCE_DIR}/3rdparty
                --html-details ${CMAKE_BINARY_DIR}/coverage/index.html
                --print-summary
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "カバレッジレポートを生成中 → ${CMAKE_BINARY_DIR}/coverage/index.html"
        )
    else()
        message(STATUS
            "gcovr が見つかりません。`coverage` ターゲットは利用できません。"
            " pip install gcovr でインストールしてください。")
    endif()
endif()

# --- サブディレクトリの読み込み -------------------------------------
# 各サブディレクトリの CMakeLists.txt を読み込んで処理する。
# これにより src/ と tests/ 配下のターゲット (ライブラリ・実行ファイル)
# がビルド対象に加わる。
add_subdirectory(src)
add_subdirectory(tests)
