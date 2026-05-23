# gRPC インストール手順

本書は、gRPC v1.78.1 をビルドし、本リポジトリの `3rdparty/` 配下に取り込むための手順を示す。

最終的な配置は以下のとおりとする。

- ヘッダ群: `3rdparty/include/grpc/`
- ライブラリ(静的ライブラリ等): `3rdparty/lib/`

なお gRPC は依存ライブラリ(Protobuf, abseil, c-ares, re2, zlib, OpenSSL/BoringSSL, upb 等)も併せてビルドされる。インストール時にはこれらのヘッダ・ライブラリも `3rdparty/include/`・`3rdparty/lib/` 配下にコピーされる点に留意すること。

---

## 0. 共通事項

### 0-1. 必要なツール

- Git
- CMake 3.16 以上
- C++17 対応のコンパイラ
  - Windows: TDM-GCC 10.3.0 (MinGW-w64, x86_64-w64-mingw32) を使用する
  - Linux: GCC 9 以上 もしくは Clang 10 以上
- (Linux のみ) `build-essential`, `autoconf`, `libtool`, `pkg-config`

### 0-2. ソースコードの取得

任意の作業ディレクトリ(本書では `work/` とする。リポジトリの外に置くことを推奨)で、以下を実行する。

```
git clone --recurse-submodules -b v1.78.1 --depth 1 --shallow-submodules https://github.com/grpc/grpc
```

以降の手順は、取得した `grpc/` ディレクトリをカレントとして実行する。

### 0-3. 用語

本書では、本リポジトリのルートディレクトリ(`CLAUDE.md` がある場所)を `<REPO>` と表記する。
最終成果物は以下に配置する。

- `<REPO>/3rdparty/include/grpc/` — gRPC のヘッダ群
- `<REPO>/3rdparty/lib/` — gRPC とその依存ライブラリ

---

## 1. Windows 版 (TDM-GCC / MinGW-w64)

TDM-GCC 10.3.0 (MinGW-w64) + CMake を用いて、x64 / Release 構成の静的ライブラリをビルドする。
PowerShell から実行することを想定する。

> 前提: TDM-GCC は `C:\TDM-GCC-64\` に、CMake は `C:\Program Files\CMake\bin\` に導入済みであることを前提とする。

### 1-1. ビルド用の作業変数を定義する

任意の作業ディレクトリ(リポジトリ外を推奨)に `grpc/` を clone した上で、その親ディレクトリで以下を実行する。`<REPO>` は本リポジトリのルートの絶対パスに置き換える。

> **重要**: 以降の手順 1-2 〜 1-4 は、**同じ PowerShell ウィンドウを閉じずに**続けて実行すること。
> ウィンドウを閉じると `$RepoRoot` や `$InstallDir` などの変数が消え、後の手順でエラーになる。

```powershell
$env:Path  = "C:\TDM-GCC-64\bin;C:\Program Files\CMake\bin;" + $env:Path
$RepoRoot   = "<REPO>"
$InstallDir = Join-Path $env:TEMP "grpc_install"   # 一旦ここへインストールし、後で 3rdparty/ へ振り分ける
```

### 1-2. CMake 構成

`grpc/` ディレクトリで以下を実行する。

```powershell
cmake -S . -B build `
  -G "MinGW Makefiles" `
  -DCMAKE_C_COMPILER="C:/TDM-GCC-64/bin/gcc.exe" `
  -DCMAKE_CXX_COMPILER="C:/TDM-GCC-64/bin/g++.exe" `
  -DCMAKE_MAKE_PROGRAM="C:/TDM-GCC-64/bin/mingw32-make.exe" `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_INSTALL_PREFIX="$InstallDir" `
  -DgRPC_INSTALL=ON `
  -DgRPC_BUILD_TESTS=OFF `
  -DgRPC_BUILD_GRPC_CSHARP_PLUGIN=OFF `
  -DgRPC_BUILD_GRPC_NODE_PLUGIN=OFF `
  -DgRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN=OFF `
  -DgRPC_BUILD_GRPC_PHP_PLUGIN=OFF `
  -DgRPC_BUILD_GRPC_PYTHON_PLUGIN=OFF `
  -DgRPC_BUILD_GRPC_RUBY_PLUGIN=OFF `
  -DBUILD_SHARED_LIBS=OFF `
  -DABSL_PROPAGATE_CXX_STD=ON `
  -DCMAKE_CXX_STANDARD=17 `
  -DOPENSSL_NO_ASM=1 `
  -DCARES_BUILD_TOOLS=OFF
```

> 備考
> - `gRPC_*_DEP_PROVIDER` は既定の `module` のままとし、submodule で同梱された依存ライブラリ(Protobuf, abseil, BoringSSL, c-ares, re2, zlib, upb)を使用する。
> - MinGW では `-G "MinGW Makefiles"` を用いる。Ninja を入れている場合は `-G Ninja` のほうがビルドが速い。
> - `-DOPENSSL_NO_ASM=1` は **必須**。BoringSSL の Windows ビルドは既定で NASM を必要とするが、TDM-GCC には同梱されないため、これを指定して BoringSSL を C 実装にフォールバックさせる。
> - `-DCARES_BUILD_TOOLS=OFF` は **必須**。c-ares 同梱の CLI ツール (`ahost.exe` 等) を MinGW でリンクすると `clock_gettime` 系シンボルが多重定義となりビルドが失敗する。これらツールは gRPC 本体のリンクには不要。

### 1-3. ビルドとインストール

```powershell
cmake --build build -j
cmake --install build
```

`$InstallDir` 配下に `include/`, `lib/`, `bin/`, `share/`, `cmake/` などが生成される。

### 1-4. `<REPO>/3rdparty/` への配置

PowerShell で以下を実行する。

```powershell
$Dst3rd = Join-Path $RepoRoot "3rdparty"
New-Item -ItemType Directory -Force -Path (Join-Path $Dst3rd "include") | Out-Null
New-Item -ItemType Directory -Force -Path (Join-Path $Dst3rd "lib")     | Out-Null
New-Item -ItemType Directory -Force -Path (Join-Path $Dst3rd "bin")     | Out-Null

# ヘッダ: include/ 配下を丸ごとコピー(grpc, grpcpp, google, absl, upb, re2, ... を含む)
Copy-Item -Recurse -Force `
  -Path (Join-Path $InstallDir "include\*") `
  -Destination (Join-Path $Dst3rd "include")

# ライブラリ: MinGW 静的ライブラリ (*.a) を 3rdparty/lib へフラットに配置
Copy-Item -Force `
  -Path (Join-Path $InstallDir "lib\*.a") `
  -Destination (Join-Path $Dst3rd "lib")

# protoc / grpc_cpp_plugin など、コード生成に必要な実行ファイルも併せて配置
Copy-Item -Force `
  -Path (Join-Path $InstallDir "bin\*.exe") `
  -Destination (Join-Path $Dst3rd "bin")
```

> ユーザ要件上、ヘッダの主目的は `3rdparty/include/grpc/` への配置だが、gRPC は `grpcpp/`, `google/protobuf/`, `absl/`, `upb/` 等の **兄弟ディレクトリ** のヘッダを `#include` で参照する。これらを失うとビルドできなくなるため、`include/` 配下は一括コピーする方針とする。

### 1-5. 動作確認

`<REPO>/3rdparty/include/grpcpp/grpcpp.h` および `<REPO>/3rdparty/lib/libgrpc++.a` が存在することを確認する。

```powershell
Test-Path (Join-Path $RepoRoot "3rdparty\include\grpcpp\grpcpp.h")
Test-Path (Join-Path $RepoRoot "3rdparty\lib\libgrpc++.a")
```

### 1-6. トラブルシュート (MinGW 固有)

- **`pthread` 関連リンクエラー**: gRPC は `winpthreads` の使用を想定する。TDM-GCC 標準同梱の `libwinpthread-1.dll` (POSIX スレッドモデル) を使用していることを確認する (`g++ -v` の出力の `Thread model:` が `posix` であること)。`win32` モデルしか入っていないビルドではリンクが通らない。
- **`<atomic>` / `__sync_*` 警告**: ABI 警告が大量に出ることがあるが、ビルド自体は通る。
- **BoringSSL のアセンブリ**: MinGW では一部の最適化アセンブリが無効化されるが、CMake が自動的に C 実装にフォールバックする。
- **リンク時の `multiple definition`**: `-Wl,--allow-multiple-definition` を `CMAKE_EXE_LINKER_FLAGS` に渡して回避するケースがある。本プロジェクトのアプリ側でも、`grpc++` をリンクする際に必要となる場合がある。

---

## 2. Linux 版

GCC + Make を用いて、Release 構成の静的ライブラリをビルドする。
bash から実行することを想定する。

### 2-1. 必要パッケージの導入(Ubuntu/Debian の例)

```bash
sudo apt-get update
sudo apt-get install -y build-essential autoconf libtool pkg-config cmake git
```

### 2-2. ビルド用の作業変数を定義する

`grpc/` ディレクトリで以下を実行する。`<REPO>` は実環境の絶対パスに置き換える。

> **重要**: 以降の手順 2-3 〜 2-5 は、**同じターミナル(シェルセッション)を閉じずに**続けて実行すること。
> ターミナルを閉じると `REPO_ROOT` や `INSTALL_DIR` などの変数が消え、後の手順でコマンドが正しく動かなくなる。

```bash
REPO_ROOT=<REPO>
INSTALL_DIR=$(mktemp -d -t grpc_install_XXXX)
```

### 2-3. CMake 構成

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" \
  -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
  -DgRPC_INSTALL=ON \
  -DgRPC_BUILD_TESTS=OFF \
  -DgRPC_BUILD_GRPC_CSHARP_PLUGIN=OFF \
  -DgRPC_BUILD_GRPC_NODE_PLUGIN=OFF \
  -DgRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN=OFF \
  -DgRPC_BUILD_GRPC_PHP_PLUGIN=OFF \
  -DgRPC_BUILD_GRPC_PYTHON_PLUGIN=OFF \
  -DgRPC_BUILD_GRPC_RUBY_PLUGIN=OFF \
  -DBUILD_SHARED_LIBS=OFF \
  -DABSL_PROPAGATE_CXX_STD=ON \
  -DCMAKE_CXX_STANDARD=17
```

> 備考
> - 共有ライブラリ(`.so`)で配布したい場合は `-DBUILD_SHARED_LIBS=ON` に変更する。
> - `CMAKE_POSITION_INDEPENDENT_CODE=ON` は、本プロジェクト側を `-fPIC` でビルドする想定で付けている。

### 2-4. ビルドとインストール

```bash
cmake --build build -j"$(nproc)"
cmake --install build
```

`${INSTALL_DIR}` 配下に `include/`, `lib/` (一部ディストリビューションでは `lib64/`), `bin/`, `share/` が生成される。

### 2-5. `<REPO>/3rdparty/` への配置

```bash
DST_3RD="${REPO_ROOT}/3rdparty"
mkdir -p "${DST_3RD}/include" "${DST_3RD}/lib" "${DST_3RD}/bin"

# ヘッダ: include/ 配下を丸ごとコピー
cp -a "${INSTALL_DIR}/include/." "${DST_3RD}/include/"

# ライブラリ: 静的ライブラリ (.a) を 3rdparty/lib へフラットに配置
#   ※ ディストリビューションによっては lib64/ に出力されるため両方を見る
#   ※ -DBUILD_SHARED_LIBS=OFF でビルドしても vendored zlib などが .so を生成する場合があるが、
#      静的リンクには不要なため .a のみをコピーする
for libdir in "${INSTALL_DIR}/lib" "${INSTALL_DIR}/lib64"; do
  if [ -d "${libdir}" ]; then
    find "${libdir}" -maxdepth 1 -type f -name '*.a' \
      -exec cp -a {} "${DST_3RD}/lib/" \;
  fi
done

# protoc / grpc_cpp_plugin など
if [ -d "${INSTALL_DIR}/bin" ]; then
  cp -a "${INSTALL_DIR}/bin/." "${DST_3RD}/bin/"
fi
```

> Windows 版と同じく、`3rdparty/include/grpc/` だけでなく `grpcpp/`, `google/`, `absl/`, `upb/`, `re2/` 等の兄弟ディレクトリ群も併せて配置する必要がある。

### 2-6. 動作確認(任意)

`<REPO>/3rdparty/include/grpcpp/grpcpp.h` および `<REPO>/3rdparty/lib/libgrpc++.a` が存在することを確認する。

---

## 3. 配置後の確認用ディレクトリ構成

正常にインストールされた場合、`<REPO>/3rdparty/` 配下はおおむね以下のようになる(主要なもののみ抜粋)。

```
3rdparty/
├── INSTALL_grpc.md
├── bin/                          # protoc, grpc_cpp_plugin など
│   ├── protoc(.exe)
│   └── grpc_cpp_plugin(.exe)
├── include/
│   ├── catch2/                   # 既存(Catch2 v3)
│   ├── nlohmann/                 # 既存
│   ├── grpc/                     # gRPC C コアのヘッダ
│   ├── grpcpp/                   # gRPC C++ のヘッダ
│   ├── google/                   # protobuf, rpc, api 等のヘッダ
│   ├── absl/                     # abseil
│   ├── upb/
│   ├── re2/
│   └── ...
└── lib/
    ├── (Windows) grpc++.lib, grpc.lib, gpr.lib, libprotobuf.lib,
    │             absl_*.lib, upb_*.lib, re2.lib, cares.lib, zlib.lib,
    │             ssl.lib, crypto.lib, address_sorting.lib, ...
    └── (Linux)   libgrpc++.a, libgrpc.a, libgpr.a, libprotobuf.a,
                  libabsl_*.a, libupb_*.a, libre2.a, libcares.a, libz.a,
                  libssl.a, libcrypto.a, libaddress_sorting.a, ...
```

依存ライブラリのファイル数はバージョンや構成オプションにより変動するため、上記は代表例である。

---

## 4. 後片付け

ビルド用に作成した `grpc/` クローンと `$InstallDir` / `${INSTALL_DIR}` は、配置確認後に削除して構わない。
