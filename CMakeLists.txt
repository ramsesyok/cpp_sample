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

# --- ビルドタイプの既定値 -------------------------------------------
# Makefile / Ninja のような単一構成ジェネレータでは CMAKE_BUILD_TYPE が
# 空のままだと最適化もデバッグ情報も付かない素のビルドになってしまう。
# 明示されていなければ Release を既定にする。
# Visual Studio など多構成ジェネレータでは CMAKE_CONFIGURATION_TYPES が
# 設定されるので、その場合は何もしない (構成は IDE 側で選ぶ)。
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build: Debug Release RelWithDebInfo MinSizeRel" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        Debug Release RelWithDebInfo MinSizeRel)
endif()

# --- C++ の言語設定 -------------------------------------------------
# 使用する C++ 標準のバージョン (ここでは C++17)。
set(CMAKE_CXX_STANDARD 17)
# ON にすると「C++17 が使えないコンパイラならエラー」になる。
# OFF (既定) だと黙って古い標準に落とされる可能性があり危険。
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# OFF にすると GNU 拡張 (-std=gnu++17 等) を使わず、純粋な ISO C++ になる。
# 移植性を高めたいので OFF が無難。
set(CMAKE_CXX_EXTENSIONS OFF)

# --- コンパイルデータベースの出力 ------------------------------------
# clang-tidy / clangd (LSP) / Include-What-You-Use 等のツールが参照する
# compile_commands.json を ${CMAKE_BINARY_DIR} に生成する。
# Makefile / Ninja ジェネレータのみ有効。Visual Studio は独自の DB を持つ。
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- テストの有効化 -------------------------------------------------
# CTest を有効にするおまじない。
# これを書いておくと、後で各 CMakeLists.txt で add_test() したものが
# `ctest` コマンドから一覧・実行できるようになる。
# ※ ルート側で 1 回だけ呼ぶのが鉄則。
enable_testing()

# --- 警告フラグ (静的解析の前段) ------------------------------------
# clang-tidy / Coverity (--all --all-security) と組み合わせて使う前提で、
# コンパイラ自身にも強めの警告を出させる。
# 使い方: 自プロジェクトのターゲットに target_link_libraries(... PRIVATE warning_flags)
# 3rdparty (catch2_main など) には付けないこと。
add_library(warning_flags INTERFACE)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(warning_flags INTERFACE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wcast-align
        -Wunused
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        -Wimplicit-fallthrough
    )
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(warning_flags INTERFACE
            -Wmisleading-indentation
            -Wduplicated-cond
            -Wduplicated-branches
            -Wlogical-op
            -Wuseless-cast
        )
    endif()
elseif(MSVC)
    # MSVC でも同等の厳しさを狙う。/permissive- は標準準拠の解釈を強制。
    target_compile_options(warning_flags INTERFACE
        /W4
        /permissive-
        /w14242  # 'identifier': conversion from 'type1' to 'type2', possible loss of data
        /w14254  # 'operator': conversion from 'type1:field_bits' to 'type2:field_bits'
        /w14263  # 'function': member function does not override any base class virtual member function
        /w14265  # 'classname': class has virtual functions, but destructor is not virtual
        /w14287  # 'operator': unsigned/negative constant mismatch
        /w14296  # 'operator': expression is always 'boolean_value'
        /w14311  # 'variable': pointer truncation from 'type1' to 'type2'
        /w14545  # expression before comma evaluates to a function which is missing an argument list
        /w14546  # function call before comma missing argument list
        /w14547  # 'operator': operator before comma has no effect; expected operator with side-effect
        /w14549  # 'operator': operator before comma has no effect; did you intend 'operator'?
        /w14555  # expression has no effect; expected expression with side-effect
        /w14619  # pragma warning: there is no warning number 'number'
        /w14640  # Enable warning on thread un-safe static member initialization
        /w14826  # Conversion from 'type1' to 'type2' is sign-extended
        /w14905  # wide string literal cast to 'LPSTR'
        /w14906  # string literal cast to 'LPWSTR'
        /w14928  # illegal copy-initialization; more than one user-defined conversion has been implicitly applied
    )
endif()

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
    # --coverage は GCC/Clang 専用フラグ。MSVC でこれを渡すと
    # コンパイラが認識できず黙って壊れる、もしくは別の意味に解釈されるため、
    # 早期にエラーで止める。
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        message(FATAL_ERROR
            "ENABLE_COVERAGE=ON は GCC/Clang 専用です。"
            " 現在のコンパイラ: ${CMAKE_CXX_COMPILER_ID}")
    endif()
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(WARNING
            "ENABLE_COVERAGE=ON ですが CMAKE_BUILD_TYPE が Debug ではありません。"
            " 最適化によりカバレッジ結果が不正確になる可能性があります。"
            " -DCMAKE_BUILD_TYPE=Debug の併用を推奨します。")
    endif()
    target_compile_options(coverage_flags INTERFACE --coverage)
    target_link_options(coverage_flags INTERFACE --coverage)

    # gcovr が見つかれば HTML レポートを生成する `coverage` ターゲットを追加。
    # gcovr 本体は PATH 上の実行ファイルとして探す。
    # 見つからなければ `python -m gcovr` でのフォールバックを試みる
    # (pip install したものが Scripts/ を PATH に通していない環境向け)。
    find_program(GCOVR_EXECUTABLE gcovr)
    if(NOT GCOVR_EXECUTABLE)
        find_package(Python3 COMPONENTS Interpreter QUIET)
        if(Python3_Interpreter_FOUND)
            execute_process(
                COMMAND ${Python3_EXECUTABLE} -m gcovr --version
                RESULT_VARIABLE _gcovr_module_check
                OUTPUT_QUIET ERROR_QUIET
            )
            if(_gcovr_module_check EQUAL 0)
                set(GCOVR_COMMAND ${Python3_EXECUTABLE} -m gcovr)
            endif()
        endif()
    else()
        set(GCOVR_COMMAND ${GCOVR_EXECUTABLE})
    endif()

    if(GCOVR_COMMAND)
        # OS により gcov 実装を切り替える。
        #   Windows: llvm-mingw/Clang ベース想定 → `llvm-cov gcov` を使う
        #   Linux:   GCC ベース想定 → 標準の `gcov` を使う
        # gcovr の --gcov-executable は文字列 1 つを受けるので、
        # 引数付き ("llvm-cov gcov") のときはそのまま 1 トークンで渡す。
        if(WIN32)
            set(GCOV_EXECUTABLE_FOR_GCOVR "llvm-cov gcov")
        else()
            set(GCOV_EXECUTABLE_FOR_GCOVR "gcov")
        endif()

        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
            COMMAND ${GCOVR_COMMAND}
                --root ${CMAKE_SOURCE_DIR}
                --filter ${CMAKE_SOURCE_DIR}/src/
                --exclude ${CMAKE_SOURCE_DIR}/3rdparty
                --exclude ${CMAKE_SOURCE_DIR}/tests/catch2
                --gcov-executable ${GCOV_EXECUTABLE_FOR_GCOVR}
                --html-details ${CMAKE_BINARY_DIR}/coverage/index.html
                --print-summary
                ${CMAKE_BINARY_DIR}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "カバレッジレポートを生成中 (gcov=${GCOV_EXECUTABLE_FOR_GCOVR}) → ${CMAKE_BINARY_DIR}/coverage/index.html"
        )
    else()
        message(STATUS
            "gcovr が見つかりません。`coverage` ターゲットは利用できません。"
            " pip install gcovr でインストールしてください。")
    endif()
endif()

# --- clang-tidy による静的解析 ---------------------------------------
# 使い方 (ビルド時に自動解析):
#   cmake -S . -B build -DENABLE_CLANG_TIDY=ON
#   cmake --build build
# 使い方 (tidy ターゲットで任意実行):
#   cmake -S . -B build
#   cmake --build build --target tidy
#
# ENABLE_CLANG_TIDY=ON にすると全ターゲットの CXX_CLANG_TIDY プロパティが
# セットされ、コンパイル時に clang-tidy が走る。3rdparty ターゲット
# (catch2_main) はサブディレクトリ側で除外する。
option(ENABLE_CLANG_TIDY "ビルド時に clang-tidy を実行する" OFF)

find_program(CLANG_TIDY_EXECUTABLE
    NAMES clang-tidy-19 clang-tidy-18 clang-tidy-17 clang-tidy-16 clang-tidy-15 clang-tidy
)

if(ENABLE_CLANG_TIDY)
    if(NOT CLANG_TIDY_EXECUTABLE)
        message(FATAL_ERROR
            "ENABLE_CLANG_TIDY=ON ですが clang-tidy が見つかりません。\n"
            "  sudo apt install clang-tidy  (Ubuntu)\n"
            "  brew install llvm            (macOS)\n"
            "でインストールしてください。")
    endif()
    message(STATUS "clang-tidy (ビルド統合): ${CLANG_TIDY_EXECUTABLE}")
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXECUTABLE}")
endif()

# tidy ターゲット: ENABLE_CLANG_TIDY の ON/OFF にかかわらず利用できる。
# run-clang-tidy (Python スクリプト) を使って src/ 以下を一括解析する。
# cmake --build build --target tidy
#
# run-clang-tidy は常に Python スクリプトとして提供される。
# LLVM-mingw など run-clang-tidy.py が同梱されない環境では、
# スクリプトを手動で入手して RUN_CLANG_TIDY_SCRIPT に指定する:
#   cmake -S . -B build -DRUN_CLANG_TIDY_SCRIPT=/path/to/run-clang-tidy.py
# スクリプトの入手先:
#   https://raw.githubusercontent.com/llvm/llvm-project/main/clang-tools-extra/clang-tidy/tool/run-clang-tidy.py
set(RUN_CLANG_TIDY_SCRIPT "" CACHE FILEPATH
    "run-clang-tidy.py のパス (自動検出できない環境で明示指定する)")

if(CLANG_TIDY_EXECUTABLE)
    find_package(Python3 COMPONENTS Interpreter QUIET)

    # 1) PATH 上の run-clang-tidy シム (Linux/macOS の apt/brew 等)
    find_program(_run_clang_tidy_in_path
        NAMES run-clang-tidy-19 run-clang-tidy-18 run-clang-tidy-17
              run-clang-tidy-16 run-clang-tidy-15 run-clang-tidy
    )

    # 2) clang-tidy と同じ bin/ にある run-clang-tidy.py
    #    (公式 LLVM Windows インストーラ, Homebrew llvm 等)
    get_filename_component(_clang_tidy_dir "${CLANG_TIDY_EXECUTABLE}" DIRECTORY)
    find_file(_run_clang_tidy_beside_clang
        NAMES run-clang-tidy.py
        HINTS "${_clang_tidy_dir}"
        NO_DEFAULT_PATH
    )

    # 3) ユーザーが明示指定したパス (LLVM-mingw 等で手動入手した場合)
    if(RUN_CLANG_TIDY_SCRIPT)
        set(_run_clang_tidy_py "${RUN_CLANG_TIDY_SCRIPT}")
    elseif(_run_clang_tidy_beside_clang)
        set(_run_clang_tidy_py "${_run_clang_tidy_beside_clang}")
    endif()

    # コマンドを確定する
    if(_run_clang_tidy_in_path)
        # PATH 上のシムはそのまま実行できる
        set(_run_clang_tidy_cmd "${_run_clang_tidy_in_path}")
    elseif(_run_clang_tidy_py AND Python3_Interpreter_FOUND)
        # .py スクリプトを Python3 経由で実行
        set(_run_clang_tidy_cmd "${Python3_EXECUTABLE}" "${_run_clang_tidy_py}")
        message(STATUS "run-clang-tidy: Python 経由で実行 → ${_run_clang_tidy_cmd}")
    endif()

    if(_run_clang_tidy_cmd)
        add_custom_target(tidy
            COMMAND ${_run_clang_tidy_cmd}
                -p "${CMAKE_BINARY_DIR}"
                "${CMAKE_SOURCE_DIR}/src"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            COMMENT "clang-tidy で静的解析中 (src/) ..."
            VERBATIM
        )
    else()
        message(STATUS
            "run-clang-tidy.py が見つかりません。`tidy` ターゲットは利用できません。\n"
            "  Ubuntu/Debian: sudo apt install clang-tidy\n"
            "  macOS:         brew install llvm\n"
            "  LLVM-mingw など: 下記からスクリプトを入手し "
            "-DRUN_CLANG_TIDY_SCRIPT=/path/to/run-clang-tidy.py で指定\n"
            "  https://raw.githubusercontent.com/llvm/llvm-project/main"
            "/clang-tools-extra/clang-tidy/tool/run-clang-tidy.py")
    endif()
else()
    message(STATUS
        "clang-tidy が見つかりません。ENABLE_CLANG_TIDY と `tidy` ターゲットは利用できません。")
endif()

# --- サブディレクトリの読み込み -------------------------------------
# 各サブディレクトリの CMakeLists.txt を読み込んで処理する。
# これにより src/ と tests/ 配下のターゲット (ライブラリ・実行ファイル)
# がビルド対象に加わる。
add_subdirectory(src)
add_subdirectory(tests)
