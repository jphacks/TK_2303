# ホッとお風呂

![header](images/header.png)

## 製品概要
### 背景
少子高齢化の進行により来年には総人口の30％が高齢者(65歳以上)になることが見込まれています。そのため、高齢者の一人暮らしや老老介護が社会問題となっています。

お風呂は非常に"不安"な場面の一つで、ヒートショックや溺死といった死亡リスクがかなり高くなります。消費者庁の統計によると、お風呂場での年間の死亡者数は19,000人にも上ります。

そのようなお風呂という「危険な」場所を「ホッと一息できる」場所にしたいという思いでこのプロダクトを制作しました。

### 製品説明
このプロダクトは、ハードウェア、サーバー、モバイルアプリからなります。

ハードウェアはお風呂に取り付けることができ、気圧、温度、湿度、CO2、音圧センサが実装されています。それらのデータから、お風呂に入っているかどうかという状態を推定し、音圧から転倒などの危険を検知します。

長くお風呂に入っている時や大きな音が出た時は、サーバーを通してユーザーにそのことが通知されます。これにより事故を未然に防いだり、万が一事故が発生したとしてもいち早く察知することができます。

![device and app](images/app_and_device.png)

### 特長
#### 1. プライバシー保護
お風呂はプライバシーが尊重される場所なので、カメラやミリ波レーダセンサ、マイクなどを設置することに抵抗を感じる方も多いでしょう。そこで音圧や気圧、湿度の情報から入浴中かどうかのステータスを検知し、必要最低限の情報をサーバーに送っています。

#### 2. モバイルアプリでのリアルタイムな危険検知
お風呂で異常が検知された場合、即座にスマートフォンにPush通知が送られ、異常に気づくことができます。そして、予め登録した緊急連絡先に電話をすることが可能になっています。

アプリを使い慣れないユーザーにとっても使いやすく、音と振動、アニメーションを有効に使いインタラクティブなUI/UXを実現しています。

#### 3. 高精度な入浴、異常検知
テストを繰り返し、入浴や異常を高精度に検知し、サーバーにデータを送っています。

### 解決出来ること
このプロダクトにより、お風呂場における高齢者の事故を検知することができ、死亡リスクを激減させられると考えています。

### 今後の展望
このプロダクトを発展させて、家全体で転倒等による事故を検出できるようにし「高齢者の方のためのスマートホーム」を実現したいと考えています。

### 注力したこと
* ハッカソンの期間内に、ユーザーがどんな課題を持っていて、どのようにアプローチするといいか検討を入念に行なったこと
* 開発メンバー3人でアプリ、サーバ、ハードウェアを分担し、それぞれのインターフェースについての情報を共有することで効率良く開発を進めたこと

## 開発技術
### 活用した技術

![architecture](images/architecture.png)

#### アプリ
* Swift
* Apple Push Notification Server

#### サーバ
* Cloudflare Workers
* Workers KV
* Firebase Cloud Messaging

#### デバイス
* ESP32
* 温度、気圧、湿度、CO2センサ(BME280)
* PDMマイク(SPH0641LU4H)
* FreeRTOS

### 独自技術
#### ハッカソンで開発した独自機能・技術
* 独自で開発したものの内容をこちらに記載してください
* [ESP32でPDMマイクを直接読み取るコード](https://github.com/jphacks/TK_2303/blob/master/hardware/device/Mic.h)
  * ESP32のライブラリの仕様が変わった影響でネット上の情報が当てにならず苦労した
