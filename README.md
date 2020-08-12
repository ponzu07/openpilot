[](#)![](https://i.imgur.com/UelUjKAh.png)

# 目次

- [オープンパイロットとは?](#what-is-openpilot)
- [純正機能との統合](#integration-with-stock-features)
- [対応ハードウェア](#supported-hardware)
- [対応車両](#supported-cars)
- [コミュニティ維持の車と特徴](#community-maintained-cars-and-features)
- [インストール手順](#installation-instructions)
- [オープンパイロットのALCとLDWの限界](#limitations-of-openpilot-alc-and-ldw)
- [オープンパイロットのACCとFCWの限界](#limitations-of-openpilot-acc-and-fcw)
- [オープンパイロットのDMの限界](#limitations-of-openpilot-dm)
- [ユーザーデータとコンマアカウント](#user-data-and-comma-account)
- [安全性とテスト](#safety-and-testing)
- [PC上でのテスト](#testing-on-pc)
- [コミュニティと貢献](#community-and-contributing)
- [ディレクトリ構造](#directory-structure)
- [ライセンス](#licensing)

---

## オープンパイロットとは?

[オープンパイロット](http://github.com/commaai/openpilot) はオープンソースの運転支援システムです。現在、openpilotは、アダプティブクルーズコントロール（ACC）、自動レーンセンタリング（ALC）、前方衝突警告（FCW）、車線逸脱警告（LDW）の機能を、サポートされている [車種、モデル、年式](#supported-cars)に合わせて実行します。さらに、オープンパイロットが作動している間は、カメラベースのドライバーモニタリング（DM）機能が、注意力散漫なドライバーや居眠り中のドライバーに警告を発します。

<table>
  <tr>
    <td>
<a href="https://www.youtube.com/watch?v=mgAbfr42oI8" title="YouTube" rel="noopener"></a><img src="https://i.imgur.com/kAtT6Ei.png">
</td>
    <td>
<a href="https://www.youtube.com/watch?v=394rJKeh76k" title="YouTube" rel="noopener"></a><img src="https://i.imgur.com/lTt8cS2.png">
</td>
    <td>
<a href="https://www.youtube.com/watch?v=1iNOc3cq8cs" title="YouTube" rel="noopener"></a><img src="https://i.imgur.com/ANnuSpe.png">
</td>
    <td>
<a href="https://www.youtube.com/watch?v=Vr6NgrB-zHw" title="YouTube" rel="noopener"></a><img src="https://i.imgur.com/Qypanuq.png">
</td>
  </tr>
  <tr>
    <td>
<a href="https://www.youtube.com/watch?v=Ug41KIKF0oo" title="YouTube" rel="noopener"></a><img src="https://i.imgur.com/3caZ7xM.png">
</td>
    <td>
<a href="https://www.youtube.com/watch?v=NVR_CdG1FRg" title="YouTube" rel="noopener"></a><img src="https://i.imgur.com/bAZOwql.png">
</td>
    <td>
<a href="https://www.youtube.com/watch?v=tkEvIdzdfUE" title="YouTube" rel="noopener"></a><img src="https://i.imgur.com/EFINEzG.png">
</td>
    <td>
<a href="https://www.youtube.com/watch?v=_P-N1ewNne4" title="YouTube" rel="noopener"></a><img src="https://i.imgur.com/gAyAq22.png">
</td>
  </tr>
</table>

## 純正機能との統合

全ての対応車両で:

- 純正レーンキープアシスト(LKA)と純正ALCは、オープンパイロットALCに置き換えられ、オープンパイロットがユーザーによって係合されている時のみ機能するようになりました。
- 純正LDWをオープンパイロットLDWに変更しました。

さらに、特定の車については、 [対応車両](#supported-cars)のACC欄を参照してください。

- 純正ACCをオープンパイロットACCに変更しました。
- オープンパイロットFCWは純正FCWの他にも動かしています。

オープンパイロットは、以下に記載されている車両の純正機能以外をすべてが機能している必要があります。<br>FCW、自動緊急ブレーキ (AEB)、オートハイビーム、死角警告(BSM)、側面衝突警告。

## 対応ハードウェア

現在、オープンパイロットは [EON DevKit](https://comma.ai/shop/products/eon-dashcam-devkit) と [コンマ2](https://comma.ai/shop/products/comma-two-devkit)をサポートしています。<br>EONやコンマ2を車に接続するには、 [カーハーネス](https://comma.ai/shop/products/car-harness) をお勧めします。 実験目的のために、openpilotは外部 [ウェブカメラ](https://github.com/commaai/openpilot/tree/master/tools/webcam)を搭載したUbuntuコンピュータ上でも動作します。

## 対応車両

メーカー | モデル(米国市場参考) | 対応パッケージ | ACC | ACC不要なアクセル制限 | 最低ALC
--- | --- | --- | --- | --- | ---
Acura | ILX 2016-18 | AcuraWatch Plus | openpilot | 25mph<sup>1</sup> | 25mph
Acura | RDX 2016-18 | AcuraWatch Plus | openpilot | 25mph<sup>1</sup> | 12mph
Honda | Accord 2018-19 | All | Stock | 0mph | 3mph
Honda | Accord Hybrid 2018-20 | All | Stock | 0mph | 3mph
Honda | Civic Hatchback 2017-19 | Honda Sensing | Stock | 0mph | 12mph
Honda | Civic Sedan/Coupe 2016-18 | Honda Sensing | openpilot | 0mph | 12mph
Honda | Civic Sedan/Coupe 2019-20 | Honda Sensing | Stock | 0mph | 2mph<sup>2</sup>
Honda | CR-V 2015-16 | Touring | openpilot | 25mph<sup>1</sup> | 12mph
Honda | CR-V 2017-20 | Honda Sensing | Stock | 0mph | 12mph
Honda | CR-V Hybrid 2017-2019 | Honda Sensing | Stock | 0mph | 12mph
Honda | Fit 2018-19 | Honda Sensing | openpilot | 25mph<sup>1</sup> | 12mph
Honda | HR-V 2019 | Honda Sensing | openpilot | 25mph<sup>1</sup> | 12mph
Honda | Insight 2019-20 | Honda Sensing | Stock | 0mph | 3mph
Honda | Odyssey 2018-20 | Honda Sensing | openpilot | 25mph<sup>1</sup> | 0mph
Honda | Passport 2019 | All | openpilot | 25mph<sup>1</sup> | 12mph
Honda | Pilot 2016-18 | Honda Sensing | openpilot | 25mph<sup>1</sup> | 12mph
Honda | Pilot 2019 | All | openpilot | 25mph<sup>1</sup> | 12mph
Honda | Ridgeline 2017-20 | Honda Sensing | openpilot | 25mph<sup>1</sup> | 12mph
Hyundai | Sonata 2020 | All | Stock | 0mph | 0mph
Lexus | CT Hybrid 2017-18 | All | Stock<sup>3</sup> | 0mph | 0mph
Lexus | ES 2019 | All | openpilot | 0mph | 0mph
Lexus | ES Hybrid 2019 | All | openpilot | 0mph | 0mph
Lexus | IS 2017-2019 | All | Stock | 22mph | 0mph
Lexus | IS Hybrid 2017 | All | Stock | 0mph | 0mph
Lexus | NX Hybrid 2018 | All | Stock<sup>3</sup> | 0mph | 0mph
Lexus | RX 2016-17 | All | Stock<sup>3</sup> | 0mph | 0mph
Lexus | RX 2020 | All | openpilot | 0mph | 0mph
Lexus | RX Hybrid 2016-19 | All | Stock<sup>3</sup> | 0mph | 0mph
Lexus | RX Hybrid 2020 | All | openpilot | 0mph | 0mph
Toyota | Avalon 2016 | TSS-P | Stock<sup>3</sup> | 20mph<sup>1</sup> | 0mph
Toyota | Avalon 2017-18 | All | Stock<sup>3</sup> | 20mph<sup>1</sup> | 0mph
Toyota | Camry 2018-20 | All | Stock | 0mph<sup>4</sup> | 0mph
Toyota | Camry Hybrid 2018-19 | All | Stock | 0mph<sup>4</sup> | 0mph
Toyota | C-HR 2017-19 | All | Stock | 0mph | 0mph
Toyota | C-HR Hybrid 2017-19 | All | Stock | 0mph | 0mph
Toyota | Corolla 2017-19 | All | Stock<sup>3</sup> | 20mph<sup>1</sup> | 0mph
Toyota | Corolla 2020 | All | openpilot | 0mph | 0mph
Toyota | Corolla Hatchback 2019-20 | All | openpilot | 0mph | 0mph
Toyota | Corolla Hybrid 2020 | All | openpilot | 0mph | 0mph
Toyota | Highlander 2017-19 | All | Stock<sup>3</sup> | 0mph | 0mph
Toyota | Highlander Hybrid 2017-19 | All | Stock<sup>3</sup> | 0mph | 0mph
Toyota | Highlander 2020 | All | openpilot | 0mph | 0mph
Toyota | Highlander Hybrid 2020 | All | openpilot | 0mph | 0mph
Toyota | Prius 2016 | TSS-P | Stock<sup>3</sup> | 0mph | 0mph
Toyota | Prius 2017-20 | All | Stock<sup>3</sup> | 0mph | 0mph
Toyota | Prius Prime 2017-20 | All | Stock<sup>3</sup> | 0mph | 0mph
Toyota | Rav4 2016 | TSS-P | Stock<sup>3</sup> | 20mph<sup>1</sup> | 0mph
Toyota | Rav4 2017-18 | All | Stock<sup>3</sup> | 20mph<sup>1</sup> | 0mph
Toyota | Rav4 2019-20 | All | openpilot | 0mph | 0mph
Toyota | Rav4 Hybrid 2016 | TSS-P | Stock<sup>3</sup> | 0mph | 0mph
Toyota | Rav4 Hybrid 2017-18 | All | Stock<sup>3</sup> | 0mph | 0mph
Toyota | Rav4 Hybrid 2019-20 | All | openpilot | 0mph | 0mph
Toyota | Sienna 2018-20 | All | Stock<sup>3</sup> | 0mph | 0mph

<sup>1</sup> [コンマペダル](https://github.com/commaai/openpilot/wiki/comma-pedal)は、現在ストップアンドゴーをサポートしていない一部のオープンパイロット対応車にストップアンドゴー機能を提供するために使用されます。 ***注：コンマペダルは[公式](https://comma.ai)にはサポートされていません。*** <br> <sup>2</sup> 2019ホンダシビック1.6Lディーゼルセダンには、12mph以下のALCはありません。 <br> <sup>3</sup>ドライバーサポートユニット（DSU）を外すと、オープンパイロットACCが純正ACCに置き換わります。 ***注：DSUを外すと自動緊急ブレーキ（AEB）が無効になります。*** <br> <sup>4</sup>フルスピードレンジのダイナミックレーダークルーズコントロールを備えていないCamry 4CYL L、4CYL LE、4CYL SEの場合は28mphになります。 <br>

## コミュニティ維持の車と特徴

メーカー | モデル(米国市場参考) | 対応パッケージ | ACC | ACC不要なアクセル制限 | 最低ALC
--- | --- | --- | --- | --- | ---
Buick | Regal 2018<sup>1</sup> | Adaptive Cruise | openpilot | 0mph | 7mph
Cadillac | ATS 2018<sup>1</sup> | Adaptive Cruise | openpilot | 0mph | 7mph
Chevrolet | Malibu 2017<sup>1</sup> | Adaptive Cruise | openpilot | 0mph | 7mph
Chevrolet | Volt 2017-18<sup>1</sup> | Adaptive Cruise | openpilot | 0mph | 7mph
Chrysler | Pacifica 2017-18 | Adaptive Cruise | Stock | 0mph | 9mph
Chrysler | Pacifica 2020 | Adaptive Cruise | Stock | 0mph | 39mph
Chrysler | Pacifica Hybrid 2017-18 | Adaptive Cruise | Stock | 0mph | 9mph
Chrysler | Pacifica Hybrid 2019-20 | Adaptive Cruise | Stock | 0mph | 39mph
Genesis | G80 2018 | All | Stock | 0mph | 0mph
Genesis | G90 2018 | All | Stock | 0mph | 0mph
GMC | Acadia Denali 2018<sup>2</sup> | Adaptive Cruise | openpilot | 0mph | 7mph
Holden | Astra 2017<sup>1</sup> | Adaptive Cruise | openpilot | 0mph | 7mph
Hyundai | Elantra 2017-19 | SCC + LKAS | Stock | 19mph | 34mph
Hyundai | Genesis 2015-16 | SCC + LKAS | Stock | 19mph | 37mph
Hyundai | Ioniq Electric Premium SE 2020 | SCC + LKAS | Stock | 0mph | 32mph
Hyundai | Ioniq Electric Limited 2019 | SCC + LKAS | Stock | 0mph | 32mph
Hyundai | Kona 2017-19 | SCC + LKAS | Stock | 22mph | 0mph
Hyundai | Kona EV 2019 | SCC + LKAS | Stock | 0mph | 0mph
Hyundai | Palisade 2020 | All | Stock | 0mph | 0mph
Hyundai | Santa Fe 2019 | All | Stock | 0mph | 0mph
Hyundai | Sonata 2019 | All | Stock | 0mph | 0mph
Hyundai | Veloster 2019 | SCC + LKAS | Stock | 5mph | 0mph
Jeep | Grand Cherokee 2016-18 | Adaptive Cruise | Stock | 0mph | 9mph
Jeep | Grand Cherokee 2019-20 | Adaptive Cruise | Stock | 0mph | 39mph
Kia | Forte 2018-19 | SCC + LKAS | Stock | 0mph | 0mph
Kia | Optima 2017 | SCC + LKAS/LDWS | Stock | 0mph | 32mph
Kia | Optima 2019 | SCC + LKAS | Stock | 0mph | 0mph
Kia | Sorento 2018 | SCC + LKAS | Stock | 0mph | 0mph
Kia | Stinger 2018 | SCC + LKAS | Stock | 0mph | 0mph
Nissan | Leaf 2018-19<sup>2</sup> | Propilot | Stock | 0mph | 0mph
Nissan | Rogue 2019<sup>2</sup> | Propilot | Stock | 0mph | 0mph
Nissan | X-Trail 2017<sup>2</sup> | Propilot | Stock | 0mph | 0mph
Subaru | Ascent 2019 | EyeSight | Stock | 0mph | 0mph
Subaru | Crosstrek 2018-19 | EyeSight | Stock | 0mph | 0mph
Subaru | Forester 2019 | EyeSight | Stock | 0mph | 0mph
Subaru | Impreza 2017-19 | EyeSight | Stock | 0mph | 0mph
Volkswagen | Golf 2015-19 | Driver Assistance | Stock | 0mph | 0mph

<sup>1</sup>[OBD-II カーハーネス](https://comma.ai/shop/products/comma-car-harness) と [コミュニティビルドのキリン](https://github.com/commaai/openpilot/wiki/GM)が必要です。***注: ASCM を外すと自動緊急ブレーキ（AEB）が無効になります。*** <br> <sup>2</sup>開発元の [カーハーネス](https://comma.ai/shop/products/car-harness)用カスタムコネクタが必要です。 <br>

上流ではありませんが、オープンパイロットをテスラで走らせるためのコミュニティがあります。 [ここ](https://tinkla.us/)

コミュニティ維持車両と特徴は、当社の [安全モデル](SAFETY.md)を満たすための検証を行っていません。使用には十分注意してください。これらは、設定で有効にした場合でのみ利用可能です。 `Settings->Developer->Enable Community Features`

コミュニティが整備した車を普及させるには、いくつかの要件を満たす必要があります。我々はブランドから1つを所有している必要があり、それのためのハーネスを販売する必要があります。パンダとオープンパイロットの両方で完全なISO26262を持っている、縦方向の制御のためのパスがある必要があります。有効なAEBを持っている必要があり、指紋2.0をサポートしている必要があります。

## インストール手順

インストーラーのセットアップ中に`https://openpilot.comma.ai` と入力して、オープンパイロットを EON またはコンマ 2 にインストールします。

[インストールビデオ](https://youtu.be/lcjqxCymins) の指示に従って、デバイスをフロントガラスに正しく取り付けてください。注: オープンパイロットにはオートポーズキャリブレーションルーチンがあり、デバイスの取り付けが不正確であることによる小さなピッチとヨーのズレがあっても、オープンパイロットのパフォーマンスに影響を与えることはありません。

デバイスをフロントガラスに取り付ける前に、運転している州や地域の法律や条例を確認してください。州法の中には、自動車のフロントガラスへの物体の設置を禁止または制限しているものがあります。

搭乗画面を確認し、キャリブレーションの手順を終えた後、オープンパイロットの操作ができるようになります。

## オープンパイロットALCとLDWの限界

オープンパイロットALC と オープンパイロットLDW は、自動的に車両を運転したり、車両を操作する際の注意力を低下させたりするものではありません。ドライバーは常にハンドルを操作し、常にオープンパイロットALCの動作を修正できるようにしておく必要があります。

車線変更中に、オープンパイロットは隣を見たり死角を確認したりすることはできません。車線変更の安全性を確認してから、ハンドルを操作して車線変更を開始してください。

多くの要因がオープンパイロットALC とオープンパイロットLDWの性能に影響を与え、意図した通りに機能しなくなることがあります。これらの要因には以下のものが含まれますが、これらだけには限定されません。

- 視界不良（大雨、雪、霧など）や、センサーの動作に支障をきたすような天候の場合。
- 道路に面したカメラが、泥、氷、雪などで遮られたり、覆われたり、損傷したりしている。
- 車両に過度の塗料や接着剤製品（ラップ、ステッカー、ゴムコーティングなど）を塗布することによって引き起こされる障害。
- デバイスが正しく取り付けられていない。
- オンオフランプ、交差点などの急カーブの場合...; オープンパイロットは、生成できるステアリングトルクの量が制限されるように設計されています。
- 制限車線や工事区域がある場合。
- 堤防の高い道路や強い横風のある場所を運転するとき。
- 非常に高温または低温。
- 明るい光（対向ヘッドライト、直射日光などによる）。
- 丘や狭い・曲がりくねった道。

上記のリストは、オープンパイロットコンポーネントの適切な操作を妨げる可能性のある状況を網羅したものではありません。常に車両をコントロールするのはドライバーの責任です。

## オープンパイロットACCとFCWの限界

オープンパイロットACC と オープンパイロットFCW は、不注意や不注意な運転を許容するシステムではありません。運転者は周囲の状況に注意を払い、常にアクセルとブレーキをコントロールしなおす準備をしておく必要があります。

多くの要因がオープンパイロットACC とオープンパイロットFCW のパフォーマンスに影響を与え、意図した通りに機能しなくなる可能性があります。これらの要因には、以下のものが含まれますが、これらに限定されません。

- センサーの動作を妨げる可能性のある視界不良（大雨、雪、霧など）または気象条件。
- 道路に面したカメラまたはレーダーが、泥、氷、雪などによって遮られたり、覆われたり、損傷したりしている。
- 車両に過度の塗料や接着剤製品（ラップ、ステッカー、ゴムコーティングなど）を塗布することによって引き起こされる障害。
- デバイスが正しく取り付けられていない。
- 料金所、橋、または大きな金属板に近づいている。
- 歩行者やサイクリストなどがいる道路を走行する場合。
- 現時点でオープンパイロットによって検出されていない交通標識または信号機がある場合。
- 投稿された速度制限がユーザーが選択した設定速度を下回る場合。現在、オープンパイロットは速度制限を検出しません。
- 同じ車線に動いていない車両がある場合。
- 急ブレーキ操作が必要な場合。 オープンパイロットは、生成できる減速と加速の量が制限されるように設計されています。
- 周囲の車両が隣接車線から接近して接近する場合。
- 丘や狭い・曲がりくねった道を運転する。
- 非常に高温または低温。
- 明るい光（対向車のヘッドライトや直射日光などによる）。
- レーダー波を生成する他の機器からの干渉。

上記のリストは、オープンパイロットコンポーネントの適切な操作を妨げる可能性のある状況を網羅したものではありません。常に車両をコントロールするのはドライバーの責任です。

## オープンパイロットDMの限界

オープンパイロットのDMは、ドライバーの警戒心を正確に測定するものではありません。

多くの要因がopenpilot DMのパフォーマンスに影響を与え、意図した通りに機能しなくなる可能性があります。これらの要因には以下のものが含まれますが、これらに限定されるものではありません。

- 夜間の運転や暗いトンネル内などの低照度条件。
- 明るい光（対向車のヘッドライトや直射日光などによる）。
- 運転者の顔が、運転者向きのカメラの視野の一部または完全に外に出ている。
- 右ハンドルの車
- 運転席向きのカメラが遮られていたり、覆われていたり、破損していたりする。

上記のリストは、オープンパイロットのコンポーネントの適切な操作を妨げる可能性のある状況を網羅したものではありません。ドライバーは、注意力のレベルを評価するためにオープンパイロットのDMに頼ってはいけません。

## ユーザーデータとコンマアカウント

初期設定では、オープンパイロットは走行データを当社のサーバーにアップロードします。また、カンマコネクトアプリとペアリングすることでデータにアクセスすることもできます。 ([iOS](https://apps.apple.com/us/app/comma-connect/id1456551889), [Android](https://play.google.com/store/apps/details?id=ai.comma.connect&hl=en_US))<br>私たちはあなたのデータを使って、より良いモデルをトレーニングし、オープンパイロットを改善していきます。

オープンパイロットはオープンソースのソフトウェアです。

オープンパイロットは、道路に面したカメラ、CAN、GPS、IMU、磁力計、温度センサー、クラッシュ、オペレーティングシステムのログを記録します。運転席カメラは、設定で明示的にオプトインした場合のみ記録されます。マイクは記録されません。

オープンパイロットを利用することで、当社の [プライバシーポリシー](https://my.comma.ai/privacy)に同意したことになります。お客様は、本ソフトウェアまたはその関連サービスの使用により、特定のタイプのユーザーデータが生成され、それらはコンマの単独の裁量で記録および保存される可能性があることを理解するものとします。本契約に同意することにより、お客様は、このデータを使用するための取消不能で永久的な全世界的な権利をコンマに付与するものとします。

## 安全性とテスト

- オープンパイロットは ISO26262 ガイドラインを遵守しています。 [SAFETY.md](SAFETY.md)
- オープンパイロットには、コミットごとに実行される[ループテスト](.github/workflows/test.yaml)のソフトウェアがあります。
- 安全モデルのコードはpandaにあり、[code rigor](https://github.com/commaai/panda#code-rigor)で書かれています。
- パンダは、ループの安全テスト [ソフトウェア](https://github.com/commaai/panda/tree/master/tests/safety)を持っています。
- 内部的には、様々なプロセスをビルドしてユニットテストを行うJenkinsのテストスイートをループ内でハードウェア化しています。
- パンダは [ループテスト](https://github.com/commaai/panda/blob/master/Jenkinsfile)で追加のハードウェアを持っています。
- 10個のEONが連続してルートを再生するテストクローゼットで最新のオープンパイロットを稼働させています。

## PC上でのテスト

マスターのツールディレクトリをチェックアウト：あなたが運転データを再生し、テストし、あなたのPCからオープンパイロットを開発するために使用することができるツールがたくさんあります。

## コミュニティと貢献

オープンパイロットは[コンマ](https://comma.ai/)とあなたのようなユーザーによって開発されています。[GitHub](http://github.com/commaai/openpilot)でのプルリクエストと課題の両方を歓迎します。バグ修正や新しいカーポートも奨励されています。

[ブランドポート](https://medium.com/@comma_ai/how-to-write-a-car-port-for-openpilot-7ce0785eda84)や[モデルポート](https://medium.com/@comma_ai/openpilot-port-guide-for-toyota-models-e5467f4b5fe6)に合わせて書いたガイドに従うことで、あなたの愛車にサポートを加えることができます。一般的には、アダプティブクルーズコントロールとレーンキープアシストが付いている車が良い候補です。私たちの[Discord](https://discord.comma.ai)に参加して、車のポートについて議論しましょう：ほとんどの車は専用のチャンネルを持っています。

オープンパイロットで仕事をしてお金をもらいたいですか？ [採用中](https://comma.ai/jobs/).

[Twitterをフォロー](https://twitter.com/comma_ai)

## ディレクトリ構造

```
.
├── apk                 # UIに使用されているapkファイル
├── cereal              # すべてのログに使用されるメッセージング仕様とlibs
├── common              # ここで開発したライブラリ的な機能
├── installer/updater   # オープンパイロットの自動更新を管理
├── opendbc             # 車のデータを解釈する方法を示すファイル
├── panda               # CAN通信に使用されるコード
├── phonelibs           # NEOSデバイスで使用するライブラリ
├── pyextra             # NEOSデバイスで使用するライブラリ
└── selfdrive           # 車を運転する際に必要なコード
    ├── assets          # UI用のフォント、画像、サウンド
    ├── athena          # アプリとの通信を可能にする
    ├── boardd          # ボードに話しかけるデーモン
    ├── camerad         # カメラセンサーから画像を取り込むためのドライバ
    ├── car             # 状態を読み取り、アクチュエータを制御するための車の特定のコード
    ├── common          # デーモン用の共有C/C++コード
    ├── controls        # Perception, planning and controls
    ├── debug           # デバッグやカーポートを行うのに役立つツール
    ├── locationd       # 近々、ホームの位置になります。
    ├── logcatd         # アンドロイドログキャット
    ├── loggerd         # 車データのロガーとアップローダー
    ├── modeld          # モデルランナーの運転と監視
    ├── proclogd        # procからの情報をログに記録する
    ├── sensord         # IMU/GPSインターフェースコード
    ├── test            # ユニットテスト、システムテスト、カーシミュレーター
    └── ui              # UI
```

サービスの相互作用を理解するには`cereal/service_list.yaml`を参照してください。

## ライセンス

オープンパイロットはMITライセンスでリリースされています。ソフトウェアの一部は、指定された他のライセンスでリリースされています。

本ソフトウェアのユーザーは、comma.ai, Inc.およびその取締役、役員、従業員、代理人、株主、関連会社、下請け業者および顧客から、ユーザーが本ソフトウェアを使用したことに起因して発生した、関連する、または結果として発生した、すべての申し立て、請求、訴訟、訴訟、訴訟、要求、損害賠償、負債、義務、損失、和解、判決、費用および費用（弁護士費用および費用を含むがこれらに限定されない）から、補償し、損害を与えないものとします。

**これは研究目的のみのためのアルファ品質のソフトウェアです。これは製品ではありません。お客様は、地域の法律および規制を遵守する責任があります。明示または暗示された保証はありません。**

---

<img src="https://d1qb2nb5cznatu.cloudfront.net/startups/i/1061157-bc7e9bf3b246ece7322e6ffe653f6af8-medium_jpg.jpg?buster=1458363130" width="75"> <img src="https://cdn-images-1.medium.com/max/1600/1*C87EjxGeMPrkTuVRVWVg4w.png" width="225">

[](https://github.com/commaai/openpilot/actions)![openpilot tests](https://github.com/commaai/openpilot/workflows/openpilot%20tests/badge.svg?event=push)
