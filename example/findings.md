# 座標計算サンプルから見えた operator / 型サポートの不足

[01_offset_from_center.cpp](01_offset_from_center.cpp), [02_bearing.cpp](02_bearing.cpp),
[03_path_simulation.cpp](03_path_simulation.cpp), [04_misc.cpp](04_misc.cpp) を実装したときに
「もう少しあると自然に書ける」と感じた点をまとめる。
本プロジェクトの目的が *リファレンス実装/構造* であることを踏まえ、
追加すべきと提案するわけではなく **観察結果** として記録する。

## 1. Vector3D 周り

| 不足機能 | 想定用途 | 代替手段 |
| --- | --- | --- |
| `Length()` / `Norm()` | 速度ベクトルの大きさ、距離の長さ | `std::sqrt(v.Dot(v))` を都度書く |
| `Normalized()` | 方位ベクトルの正規化 | 同上、Length で割る |
| `Cross()` | 法線・外積一般 | サンプル04で局所実装 |
| 単項マイナス `operator-` | ベクトル反転 (重力など) | `Vector3D{0,0,0} - v` か `v * -1.0` |
| 複合代入 `+=`, `-=`, `*=`, `/=` | ループ内での積算 | 毎回再代入 `v = v + w` |
| `operator==` / `operator!=` | テスト・等値判定 | 成分ごとに比較 |
| ストリーム出力 `operator<<` | デバッグログ | 成分を個別に出力 |

## 2. Attitude 周り

- `+=`, `-=`, `*=`, `/=` が無い。
- `==`/`!=` が無い。
- `Lerp(a, b, t)` のようなユーティリティは無いが、`a + (b - a) * t` で書ける
  (オイラー角の単純線形補間は短い時間ステップ以外では使うべきではない、という
  本質的な注意は別)。

## 3. アフィン点 (GlobalCoordinate / LocalCoordinate / BodyCoordinate)

これらは「位置同士の和は意味を持たない」という方針で設計されていることが
ヘッダコメントから明確で、それは妥当。その上で気付いた点:

- `operator+=(Vector3D)` / `operator-=(Vector3D)` が無い。等速度モデルなどで
  逐次積算するときに `p = p + v * dt` と書く必要がある。
- 「座標を Vector3D に明示変換する」関数 (例: `ToVector()`) が無い。
  サンプルでは `point - PointType{0,0,0}` で原点との差を取って Vector3D を取り出した。
  オフセット計算で BodyCoordinate を Vector3D 的に扱いたい場面が頻出するため、
  明示メソッドがあると読みやすい。
- `operator==`/`!=` が無い。
- 2 点を内分する `Lerp(a, b, t)` のような補間関数が無い。
  `a + (b - a) * t` で書けるので必須ではない。

## 4. 座標系間のフレーム変換

`coordinate_conversion.h` には以下があり、それらは整っている:

- `GeoToMap` / `MapToGeo` (rad ↔ deg)
- `GeoToGlobal` / `GlobalToGeo` (geodetic ↔ ECEF, Bowring 1976)

一方、本サンプルで必要だったのに **未提供** だったもの:

- NED (`LocalCoordinate`) ⇔ ECEF (`GlobalCoordinate`) の変換。
  参照点となる `GeoCoordinate` (lat/lon) が必要なので、関数シグネチャは
  `LocalToGlobal(local, ref_geo)` のような形になる。サンプル01, 04 で
  `NedToEcefRotation` / `EcefToNedRotation` を局所実装した。
- Body (`BodyCoordinate`) ⇔ NED (`LocalCoordinate`) の変換。
  `FromAttitude` が ZYX オイラー角の回転行列 (body → NED) を返し、
  Body 軸 = (Forward, Right, Down) と NED 軸 = (North, East, Down) で
  Z の符号も一致するため、軸並べ替えは不要で R(att) をそのまま適用できる。
  ただしフレームが型に乗っていない点は依然として注意が必要 (節6参照)。

## 5. 地表面計算ユーティリティ

サンプル02・例3 で必要になったが未提供だったもの。これらは「演算プログラム
本体に置くべきかは設計判断」だが、座標型の typical な用途として要望は出やすい:

- `InitialBearing(a, b)` / `FinalBearing(a, b)`
- `GreatCircleDistance(a, b)` (Haversine など)
- 距離・方位から相手座標を求める「逆問題」(`Destination(a, bearing, distance)`)

## 6. 型安全性に関する追加観察

- `GlobalCoordinate + Vector3D` がコンパイル可能だが、`Vector3D` が
  **どのフレーム** (ECEF 軸 / NED 軸 / Body 軸) で表されているかは型に
  乗らないため、誤って Body 軸の Vector3D を ECEF の点に足してしまう
  バグが書ける。フレームをタグ付けした `Vector3D<EcefFrame>` のような
  設計もありうるが、現在の設計方針からは外れるため要相談。
- 同様に `Matrix3x3` も「from→to のフレームのペア」を型で持っていないため、
  順方向回転と逆方向回転 (転置) を取り違える事故が起きやすい。
