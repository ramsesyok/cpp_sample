# =====================================================================
# src/common/CMakeLists.txt
#   アプリ内の複数モジュールから共通に使う処理を集めた
#   静的ライブラリ "myapp_common" を定義する。
# =====================================================================

# myapp_common 静的ライブラリ。
# ここに含めるのは「特定の業務ロジックに依存しない汎用処理」。
# 例: 幾何計算、文字列ユーティリティ、ロガー など。
add_library(myapp_common
    polygon.cpp
)

# include パスの公開設定。
#   - ${CMAKE_SOURCE_DIR}/src を PUBLIC にすることで、myapp_common を
#     リンクする側は #include "common/polygon.h" のように
#     "フォルダ名/ヘッダ名" の形で書ける。
#     これにより、どの層のヘッダなのか一目で分かる。
#   - 3rdparty ヘッダもここで通しておけば、common 内から
#     <nlohmann/json.hpp> 等を使うコードを後で追加しても困らない。
target_include_directories(myapp_common
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/3rdparty/include
)
