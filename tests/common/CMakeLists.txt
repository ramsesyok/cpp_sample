# =====================================================================
# tests/common/CMakeLists.txt
#   src/common/ にあるモジュール (myapp_common) に対する単体テストを
#   定義する。
# =====================================================================

# polygon (点の内外判定) のテスト実行ファイル。
add_executable(test_polygon test_polygon.cpp)

# テスト対象である myapp_common と、Catch2 本体をリンク。
#   - myapp_common はテストしたい当の実装
#   - catch2_main は親ディレクトリ (tests/CMakeLists.txt) で作った
#     Catch2 共有ライブラリ。サブディレクトリからも名前で参照できる。
target_link_libraries(test_polygon PRIVATE myapp_common catch2_main)

# CTest に登録。これで `ctest` 実行時に test_polygon も流れる。
add_test(NAME test_polygon COMMAND test_polygon)
