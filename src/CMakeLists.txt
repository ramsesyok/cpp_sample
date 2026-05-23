# =====================================================================
# src/CMakeLists.txt
#   アプリ本体 (myapp 実行ファイル) と、その実装を担う静的ライブラリ
#   (myapp_lib) を定義する。
# =====================================================================

# common/ サブディレクトリを先に読み込み、myapp_common ライブラリを
# 利用可能にする。後段で myapp_lib がリンクする依存先になる。
add_subdirectory(common)

# --- myapp_lib: アプリのロジックを集めた静的ライブラリ --------------
# 実行ファイルに直接 .cpp を並べず、ライブラリに切り出しておくと:
#   - テスト側からも同じビルド成果物をリンクできる
#   - main() からロジックを分離できて単体テストしやすい
# というメリットがある。
add_library(myapp_lib
    greet.cpp
)

# myapp_lib をリンクする側 (= myapp 実行ファイルやテスト) が
# どの include パスを参照できるかを定義する。
#   - PUBLIC  : 自分自身のビルドにも、リンクする側にも適用される
#   - PRIVATE : 自分のビルドだけに適用される (ヘッダを外に見せたくない時)
#   - INTERFACE: 自分には不要だが、リンクする側だけに適用したい時
# ここでは greet.h を外部 (myapp/テスト) からも使うので PUBLIC。
target_include_directories(myapp_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}                  # src/ を起点に "greet.h" を解決
        ${CMAKE_SOURCE_DIR}/3rdparty/include         # nlohmann/json などの 3rdparty ヘッダ
)

# myapp_lib は myapp_common に依存する (例えば main から共通処理を使いたい場合に備えて)。
# PUBLIC でリンクしておくと、myapp_lib にリンクするだけで myapp_common の
# ヘッダ/シンボルも自動的に解決される (依存の推移的伝播)。
target_link_libraries(myapp_lib PUBLIC myapp_common PRIVATE coverage_flags)

# --- myapp: 最終的な実行ファイル ------------------------------------
# main.cpp は薄いエントリポイントだけにし、本体ロジックは myapp_lib 側へ。
add_executable(myapp main.cpp)

# myapp 実行ファイルだけが myapp_lib を使うので PRIVATE で十分。
# (myapp にさらにリンクしてくる人はいないため)
target_link_libraries(myapp PRIVATE myapp_lib coverage_flags)
