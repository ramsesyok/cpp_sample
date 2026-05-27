# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Current state

The repository is a CMake + Catch2 ベースの C++17 プロジェクトのひな型。ビルドは [CMakeLists.txt](CMakeLists.txt) (ルート) / [src/CMakeLists.txt](src/CMakeLists.txt) / [src/common/CMakeLists.txt](src/common/CMakeLists.txt) / [tests/CMakeLists.txt](tests/CMakeLists.txt) / [tests/common/CMakeLists.txt](tests/common/CMakeLists.txt) で構成される。新しい共通モジュールやテストを足す際は既存の `add_library` / `add_executable` + `add_test` のパターンに合わせる。

[src/common/](src/common/) には演算系ユーティリティが header-only で揃っている: `vector3d.h`, `attitude.h`, `matrix3x3.h`, `global_coordinate.h` (ECEF), `local_coordinate.h` (NED), `body_coordinate.h` (機体), `geo_coordinate.h` (測地), `map_coordinate.h`, `coordinate_conversion.h`。Point 型 (Global/Local/Body) はアフィン点として扱い、提供する演算子は `Point + Vector3D = Point` / `Point - Vector3D = Point` / `Point - Point = Vector3D` のみ。Point 同士の加算や異なる Point 型間の演算は意図的に提供しない。`Matrix3x3` は `m11_` … `m33_` の 9 個の named member で持つ (行優先, 1-origin)。

[example/](example/) には座標型を実際に使ったスタンドアロンのサンプル (`01_offset_from_center.cpp` ほか) と、そこから見えた API 観察 [example/findings.md](example/findings.md) がある。ルートの CMake からは取り込んでおらず、ビルド方法は [example/README.md](example/README.md) を参照。

## Vendored dependencies

Third-party libraries live under [3rdparty/include/](3rdparty/include/) as in-tree headers, not as submodules or a package-manager manifest. Any build configuration must add `3rdparty/include` to the include path.

- **Catch2 v3 (amalgamated)** — [tests/catch2/](tests/catch2/). Both the header and the `.cpp` are present, so tests link by compiling `catch_amalgamated.cpp` into the test binary (no separate Catch2 library build, no `CATCH_CONFIG_MAIN` macro — v3 provides its own `main`). `tests/` is added to the include path via `tests/CMakeLists.txt` so `#include <catch2/catch_amalgamated.hpp>` resolves.
- **nlohmann/json (single-header)** — [3rdparty/include/nlohmann/](3rdparty/include/nlohmann/). Include `<nlohmann/json.hpp>`; `json_fwd.hpp` is available for forward declarations in headers.
