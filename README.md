# cpp_sample

CMake + Catch2 をベースにした、C++ プロジェクトの構成サンプル。
最終的には演算系アプリケーションのひな型とすることを意図しており、
ログ・依存管理・テストの構成方針をひととおり盛り込んでいる。

## このサンプルが示すこと

- **CMake + Catch2** によるアプリ本体／テストの分離構成
- **依存ライブラリは原則 [3rdparty/](3rdparty/) 配下にバンドル**
  （外部のパッケージマネージャに依存しない自己完結型）
- **spdlog による高性能ロギング** — 演算スレッドをブロックしない使い方
- **モジュール分割の最小例** — `common/` 配下に共通処理を集約

## ディレクトリ構成

```
cpp_sample/
├── CMakeLists.txt              ルート。C++ 標準・CTest 有効化・サブディレクトリ取り込み
├── README.md                   (このファイル)
├── CLAUDE.md                   Claude Code 向けのリポジトリ説明
├── 3rdparty/                   サードパーティ依存 (ヘッダ + 必要ならビルド済みライブラリ)
│   ├── INSTALL_grpc.md         gRPC のローカルインストール手順
│   ├── include/                ヘッダ群 (nlohmann / spdlog / grpc / absl 等)
│   ├── lib/                    gRPC とその依存のスタティックライブラリ群
│   └── bin/                    protoc, grpc_cpp_plugin など
├── src/
│   ├── CMakeLists.txt          myapp_lib (静的ライブラリ) + myapp (実行ファイル)
│   ├── main.cpp                エントリポイント。ロガーを RAII で init/shutdown
│   ├── greet.h / greet.cpp     サンプル機能
│   └── common/                 アプリ横断の共通処理
│       ├── CMakeLists.txt      myapp_common ライブラリ定義
│       ├── logging.h / .cpp    spdlog の初期化・shutdown
│       ├── vector3d.h          3次元ベクトル (演算サンプル、header-only)
│       ├── attitude.h          姿勢角 (ロール/ピッチ/ヨー、header-only)
│       ├── ecef_coordinate.h   ECEF 座標 (header-only)
│       ├── geo_coordinate.h    測地座標 (緯度経度高度、header-only)
│       ├── map_coordinate.h    地図座標 (header-only)
│       └── coordinate_conversion.h  座標系変換ユーティリティ (header-only)
└── tests/
    ├── CMakeLists.txt          Catch2 amalgamated を共有 static lib 化 + test_greet
    ├── catch2/                 Catch2 v3 amalgamated (header + .cpp)
    ├── test_greet.cpp
    └── common/
        ├── CMakeLists.txt      common 配下モジュールの単体テスト群
        ├── test_vector3d.cpp
        ├── test_attitude.cpp
        ├── test_ecef_coordinate.cpp
        ├── test_geo_coordinate.cpp
        └── test_coordinate_conversion.cpp
```

## ビルド & テスト

```powershell
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

個別のテストを実行する例:

```powershell
# CTest 経由 (登録名でフィルタ)
ctest --test-dir build -R test_vector3d --output-on-failure

# 直接実行 (Catch2 のタグでフィルタ)
./build/tests/common/test_vector3d "[vector3d]"
```

ビルド構成の切替 (ロガーのコンパイル時レベルが変わる):

```powershell
# Debug : SPDLOG_ACTIVE_LEVEL=TRACE  (全ログ有効)
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug

# Release: SPDLOG_ACTIVE_LEVEL=INFO  (TRACE/DEBUG はコードごと消える)
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
```

## カバレッジ (Linux / GCC・Clang)

`-DENABLE_COVERAGE=ON` を付けてビルドすると、gcov による行カバレッジ計装が有効になる。
最適化によるカウント誤差を防ぐため、`Debug` ビルドとの併用を推奨する。

```sh
cmake -S . -B build-cov -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build-cov
ctest --test-dir build-cov --output-on-failure
```

テスト実行後、各 `.cpp` の計測データ (`.gcda`) が `build-cov/` 以下に生成される。

### gcov で直接確認する

```sh
cd build-cov/src/CMakeFiles/myapp_lib.dir
gcov greet.cpp.gcda          # greet.cpp.gcov を生成して行ごとのカウントを表示
```

### gcovr で HTML レポートを生成する

[gcovr](https://gcovr.com/) をインストールすると、`coverage` ターゲットから
HTML レポートをワンコマンドで生成できる。

```sh
pip install gcovr

# cmake 再設定は不要 — すでに build-cov/ がある場合はそのまま使う
cmake --build build-cov --target coverage
# → build-cov/coverage/index.html が生成される
```

ブラウザで `build-cov/coverage/index.html` を開くとファイルごとの
カバレッジ率と未実行行が確認できる。

> **対象範囲**: `src/` 配下の本体コードのみ計測する。
> `3rdparty/` および `tests/catch2/` (Catch2 本体) は集計から除外される。

## サードパーティ依存

依存はすべて [3rdparty/](3rdparty/) 配下に置き、リポジトリ単体でビルドできる
状態を維持する方針。

| ライブラリ | 形態 | 場所 | 役割 |
|---|---|---|---|
| Catch2 v3 | amalgamated (header + .cpp) | [tests/catch2/](tests/catch2/) | 単体テストフレームワーク |
| nlohmann/json | single-header | [3rdparty/include/nlohmann/](3rdparty/include/nlohmann/) | JSON 入出力 |
| spdlog | header-only | [3rdparty/include/spdlog/](3rdparty/include/spdlog/) | ロギング (async 構成で使用) |
| gRPC + 依存群 | ビルド済みライブラリ | [3rdparty/include/](3rdparty/include/) / [3rdparty/lib/](3rdparty/lib/) / [3rdparty/bin/](3rdparty/bin/) | RPC |

- Catch2 / nlohmann/json / spdlog はヘッダ系のみで完結するため、CMake
  からは include パスを通すだけで使える。
- **gRPC は事前ビルドが必要**。手順は [3rdparty/INSTALL_grpc.md](3rdparty/INSTALL_grpc.md) を参照。
  `protoc.exe` / `grpc_cpp_plugin.exe` は [3rdparty/bin/](3rdparty/bin/)、
  リンク用 `.a` 群は [3rdparty/lib/](3rdparty/lib/) に配置済み。

## ポイントとなるファイル

### ビルドの骨格

- [CMakeLists.txt](CMakeLists.txt) — ルート。`C++17`、`CTest` 有効化、サブディレクトリ取り込みのみ。
- [src/CMakeLists.txt](src/CMakeLists.txt) — `myapp_lib`（静的ライブラリ）と `myapp`（実行ファイル）の分離。
  実装をライブラリ化することでテスト側からも同一バイナリをリンクできる。
- [src/common/CMakeLists.txt](src/common/CMakeLists.txt) — 共通処理ライブラリ `myapp_common`。
  `${CMAKE_SOURCE_DIR}/src` を `PUBLIC` include に通すことで、
  利用側は `#include "common/xxx.h"` のようにモジュール名付きで書ける。
  また `SPDLOG_ACTIVE_LEVEL` をビルド構成別に切り替えるジェネレータ式もここ。
- [tests/CMakeLists.txt](tests/CMakeLists.txt) — Catch2 v3 amalgamated を
  `catch2_main` という静的ライブラリにまとめ、各テスト実行ファイルから
  共有することで再コンパイルコストを抑えている。

### アプリ本体

- [src/main.cpp](src/main.cpp) — `LoggingGuard` で RAII 的に
  `logging::init()` / `logging::shutdown()` を確実に呼び、例外時は
  `SPDLOG_CRITICAL` で記録する最小骨格。
- [src/common/logging.h](src/common/logging.h) / [src/common/logging.cpp](src/common/logging.cpp) —
  spdlog の async ロガーを `console + rotating file` の 2 sink 構成で初期化。
  詳細は次節「ロギング方針」を参照。
- [src/common/vector3d.h](src/common/vector3d.h) —
  演算処理のサンプル (header-only)。四則演算と内積を持つ 3 次元ベクトル型。
  不変条件を持たないため `struct` で定義している例。
- [src/common/attitude.h](src/common/attitude.h) — 姿勢角 (ロール/ピッチ/ヨー) を表す header-only モジュール。
- [src/common/ecef_coordinate.h](src/common/ecef_coordinate.h) /
  [src/common/geo_coordinate.h](src/common/geo_coordinate.h) /
  [src/common/map_coordinate.h](src/common/map_coordinate.h) —
  ECEF / 測地 / 地図の各座標型 (header-only)。
- [src/common/coordinate_conversion.h](src/common/coordinate_conversion.h) —
  上記座標系間の変換ユーティリティ (header-only)。

### テスト

- [tests/test_greet.cpp](tests/test_greet.cpp) — モジュール = テスト実行ファイル
  方針の最小例。
- [tests/common/test_vector3d.cpp](tests/common/test_vector3d.cpp) — Catch2 の
  `SECTION` を使った分岐構造のサンプル。
- [tests/common/test_attitude.cpp](tests/common/test_attitude.cpp) — 姿勢角モジュールの単体テスト。
- [tests/common/test_ecef_coordinate.cpp](tests/common/test_ecef_coordinate.cpp) /
  [tests/common/test_geo_coordinate.cpp](tests/common/test_geo_coordinate.cpp) —
  各座標型の単体テスト。
- [tests/common/test_coordinate_conversion.cpp](tests/common/test_coordinate_conversion.cpp) —
  座標系変換の単体テスト。

## ロギング方針

演算性能を最優先するため、ロガーは以下のルールで運用する。

1. **コンパイル時にレベルを落とす**
   `SPDLOG_ACTIVE_LEVEL` を CMake から定義し、`SPDLOG_TRACE` / `SPDLOG_DEBUG`
   は Release では**プリプロセッサで丸ごと消える**。引数の式すら評価されない。
2. **非同期ロガーを使う** (`spdlog::async_logger`)
   呼び出しスレッドはキューに enqueue するだけで I/O は別スレッドが処理。
3. **オーバーフロー時は古いログを破棄** (`overrun_oldest`)
   キュー満杯でも演算スレッドをブロックしない。
4. **必ず `SPDLOG_xxx` マクロ経由で書く**
   `logger->info(...)` を直接呼ぶとコンパイル時フィルタの恩恵を受けないため禁止。
5. **ホットループ内ではログを書かない**
   どうしても必要な場合は `SPDLOG_TRACE` に限定し、Release で消える前提で使う。
   またはサンプリング (`if ((i & 0xFFFF) == 0) ...`) を併用する。

ロガー設定の中身（sink・パターン・キューサイズ等）は [src/common/logging.cpp](src/common/logging.cpp) を参照。

## 拡張時の指針

- **新しい共通処理を足す**: [src/common/](src/common/) にファイルを追加し、
  [src/common/CMakeLists.txt](src/common/CMakeLists.txt) の `add_library` 行にソースを追加。
  テストは [tests/common/](tests/common/) に対応する `test_xxx.cpp` を作り、
  [tests/common/CMakeLists.txt](tests/common/CMakeLists.txt) に `add_executable` + `add_test` を 1 セット書く。
- **新しいサブシステムを足す** (例: `io/`, `compute/`): `src/<name>/` 配下に
  CMakeLists.txt とソースを置き、[src/CMakeLists.txt](src/CMakeLists.txt) に
  `add_subdirectory(<name>)` を 1 行追加。テスト側も同様の階層を作る。
- **新しい 3rdparty ライブラリを足す**: 原則 [3rdparty/include/](3rdparty/include/) に
  ヘッダを置き、ビルドが必要なものは [3rdparty/lib/](3rdparty/lib/) / [3rdparty/bin/](3rdparty/bin/) を併用。
  手順が複雑なものは `3rdparty/INSTALL_xxx.md` として残す。
