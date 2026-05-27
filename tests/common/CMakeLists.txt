# =====================================================================
# tests/common/CMakeLists.txt
#   src/common/ にあるモジュール (myapp_common) に対する単体テストを
#   定義する。
# =====================================================================

# vector3d (3次元ベクトル) のテスト実行ファイル。
# ヘッダオンリーなので myapp_common へのリンクは不要だが、
# include パスを通すために PUBLIC な include を引き継ぐ目的でリンクしておく。
#   - myapp_common はテスト対象モジュールが置かれる静的ライブラリ
#   - catch2_main は親ディレクトリ (tests/CMakeLists.txt) で作った
#     Catch2 共有ライブラリ。サブディレクトリからも名前で参照できる。
add_executable(test_vector3d test_vector3d.cpp)
target_link_libraries(test_vector3d PRIVATE myapp_common catch2_main coverage_flags)

# CTest に登録。これで `ctest` 実行時に test_vector3d も流れる。
add_test(NAME test_vector3d COMMAND test_vector3d)

# attitude (姿勢角) のテスト実行ファイル。Vector3D と同様にヘッダオンリー。
add_executable(test_attitude test_attitude.cpp)
target_link_libraries(test_attitude PRIVATE myapp_common catch2_main coverage_flags)
add_test(NAME test_attitude COMMAND test_attitude)

# 座標系まわり (GlobalCoordinate(ECEF) / GeoCoordinate / MapCoordinate / 変換)。
# いずれもヘッダオンリーだが、include パスと Catch2 を引き込むため
# myapp_common と catch2_main にリンクしておく。
add_executable(test_global_coordinate test_global_coordinate.cpp)
target_link_libraries(test_global_coordinate PRIVATE myapp_common catch2_main coverage_flags)
add_test(NAME test_global_coordinate COMMAND test_global_coordinate)

add_executable(test_geo_coordinate test_geo_coordinate.cpp)
target_link_libraries(test_geo_coordinate PRIVATE myapp_common catch2_main coverage_flags)
add_test(NAME test_geo_coordinate COMMAND test_geo_coordinate)

add_executable(test_coordinate_conversion test_coordinate_conversion.cpp)
target_link_libraries(test_coordinate_conversion PRIVATE myapp_common catch2_main coverage_flags)
add_test(NAME test_coordinate_conversion COMMAND test_coordinate_conversion)

# ローカル (NED) / 機体座標系および 3x3 行列。いずれもヘッダオンリー。
add_executable(test_local_coordinate test_local_coordinate.cpp)
target_link_libraries(test_local_coordinate PRIVATE myapp_common catch2_main coverage_flags)
add_test(NAME test_local_coordinate COMMAND test_local_coordinate)

add_executable(test_body_coordinate test_body_coordinate.cpp)
target_link_libraries(test_body_coordinate PRIVATE myapp_common catch2_main coverage_flags)
add_test(NAME test_body_coordinate COMMAND test_body_coordinate)

add_executable(test_matrix3x3 test_matrix3x3.cpp)
target_link_libraries(test_matrix3x3 PRIVATE myapp_common catch2_main coverage_flags)
add_test(NAME test_matrix3x3 COMMAND test_matrix3x3)
