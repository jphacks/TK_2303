# Backendサーバー

## 概要

センサーデバイスとスマホアプリの双方と通信を行い、デバイスの状態をデータベースに保持するためのサーバー

## 機能

- デバイスの状態をデータベースに保持する
  - センサの値
    - 温度
    - 湿度
    - CO2濃度
  - デバイスのユーザー検知状態
    - 入浴中
    - 非入浴中
    - 危険
- 保持しているデータをスマホアプリに提供する
  - デバイスの状態
  - デバイスのユーザー検知状態
- デバイスからの通知をスマホアプリに転送する
  - デバイスのユーザー検知状態が危険になったとき

## 使用技術

Cloudflare Workersを利用してAPIサーバーの開発を行った。
Cloudflare WorkersはCDNエッジでJavaScriptを実行することができるサービスで、
リクエストを送信したクライアントから近い場所で処理が実行されるため、AWS LambdaやVercelなどの他のサーバーレスよりも遅延が少ないという特徴がある。

データの永続化に関しては、Cloudflare Workers KVを利用した。
Cloudflare Workersに付属するキーバリューストアで、分散化されたアーキテクチャで高頻度なアクセスも高速に処理することができる。

Cloudflare Workersはデプロイするスクリプトのサイズ制限や、Node.jsによって追加されるAPIを利用できないなどの制限があるため、
一般的なNode.jsによる実装と比べて実装が少し難しい。
特に通知機能はFirebase Cloud Messagingを利用して実装したのだが、公式で提供されているSDKはNode.jsのAPIを利用しているため、
Cloudflare Workersでは利用できない。
そのため、Firebase Cloud MessagingのAPIを利用するためのOAuth2.0の認証を自前で実装をした。

## TODO

- [ ] 認証したユーザーやデバイスのみがAPIにアクセスできるようにする

```
npm install
npm run dev
```

```
npm run deploy
```
