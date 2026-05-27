# =====================================================================
# tests/common/CMakeLists.txt
#   src/common/ にあるモジュール (myapp_common) に対する単体テストを
#   定義する。
#
#   `myapp_add_test` は親 (tests/CMakeLists.txt) で定義したヘルパ関数。
#   catch2_main / coverage_flags は自動付与されるので、各テストは
#   myapp_common にのみリンクすればよい。
# =====================================================================

myapp_add_test(test_vector2d              test_vector2d.cpp              PRIVATE_LIBS myapp_common)
myapp_add_test(test_vector3d              test_vector3d.cpp              PRIVATE_LIBS myapp_common)
myapp_add_test(test_attitude              test_attitude.cpp              PRIVATE_LIBS myapp_common)
myapp_add_test(test_global_coordinate     test_global_coordinate.cpp     PRIVATE_LIBS myapp_common)
myapp_add_test(test_geo_coordinate        test_geo_coordinate.cpp        PRIVATE_LIBS myapp_common)
myapp_add_test(test_coordinate_conversion test_coordinate_conversion.cpp PRIVATE_LIBS myapp_common)
myapp_add_test(test_local_coordinate      test_local_coordinate.cpp      PRIVATE_LIBS myapp_common)
myapp_add_test(test_body_coordinate       test_body_coordinate.cpp       PRIVATE_LIBS myapp_common)
myapp_add_test(test_matrix3x3             test_matrix3x3.cpp             PRIVATE_LIBS myapp_common)
