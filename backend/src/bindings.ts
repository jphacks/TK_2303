export type Bindings = {
  DEVICE_STATUSES: KVNamespace;
  FIREBASE_KEY_FILE: string;
  IOS_TOKEN: string;
  REGISTER_API_TOKEN: string;
  FIRMWARE_DOWNLOAD_SITE: string;
  OPENAI_API_KEY: string;
  OPENAI_ORG_ID: string;
  VAD_API_URL: string;
  DB: D1Database;
  AUDIO_BUCKET: R2Bucket;
  AI: any;
};

export type AuthVariables = {
  token: string;
  hashed: string;
};
