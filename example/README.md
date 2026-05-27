# example/

座標型 (`src/common/` 配下) を実際に使ってみて、operator や型の不足が
無いかを試すサンプルコード集。

## 方針

- 本プロジェクトはリファレンス実装/構造であり、現状の CMake に
  example/ を組み込むことはしない (ルートおよび子の `CMakeLists.txt` は
  一切変更していない)。
- 各 `.cpp` は **スタンドアロン** にビルドできる。
  使用するヘッダはすべて `src/common/` 配下のヘッダオンリーモジュール
  であり、`logging.cpp` などのリンクは不要。

## ビルド方法

リポジトリルートで:

```sh
clang++ -std=c++17 -Wall -Wextra \
    -I src -I 3rdparty/include \
    example/01_offset_from_center.cpp -o example/01_offset_from_center
```

他のファイル (`02_bearing.cpp`, `03_path_simulation.cpp`, `04_misc.cpp`)
も同様。`g++` でも同じ手順で通る (clang 22 で動作確認済み)。

## ファイル一覧

| ファイル | 内容 |
| --- | --- |
| [01_offset_from_center.cpp](01_offset_from_center.cpp) | グローバル座標 + Body 軸でのオフセット → グローバル座標 |
| [02_bearing.cpp](02_bearing.cpp) | A → B の初期方位 (bearing) と大円距離 |
| [03_path_simulation.cpp](03_path_simulation.cpp) | 通過時刻付き waypoint 列の補間による移動模擬 |
| [04_misc.cpp](04_misc.cpp) | 2 点間の NED 分解、外積による法線、姿勢補間など |
| [findings.md](findings.md) | サンプル実装を通じて見えた operator/型の不足の一覧 |
