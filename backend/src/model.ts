import { Env } from 'hono';
import {
  Alart,
  DeviceStatus,
  History,
  HistoryEntry,
  PhoneNumberData,
  SensorData,
  SensorDataWithTime,
} from './types';
import { statusToHistoryEntry } from './util';

import { SignJWT, importPKCS8 } from 'jose';

const KEY_PREFIX = 'dev:v0';
type KeyKind = 'status' | 'sensor' | 'phonenumber' | 'history';
const generateKey = (id: string, kind: KeyKind) => {
  return `${KEY_PREFIX}:${kind}:${id}`;
};

export const getStatus = async (
  KV: KVNamespace,
  id: string
): Promise<DeviceStatus> => {
  const value = await KV.get(generateKey(id, 'status'));
  if (!value) {
    return {
      status: 'unknown',
    };
  }

  const status: DeviceStatus = JSON.parse(value);
  return status;
};

export const updateStatus = async (
  KV: KVNamespace,
  id: string,
  status: DeviceStatus
) => {
  // Get old status
  const oldStatus = await getStatus(KV, id);

  // Update status
  const value = JSON.stringify(status);
  await KV.put(generateKey(id, 'status'), value);

  // If status is changed, update history
  if (status.status !== 'unknown' && oldStatus.status !== status.status) {
    const historyEntry = statusToHistoryEntry(status);
    await appendHistory(KV, id, historyEntry);
  }
};

export const getSensorData = async (
  KV: KVNamespace,
  id: string
): Promise<SensorDataWithTime> => {
  const value = await KV.get(generateKey(id, 'sensor'));
  if (!value) {
    // TODO: データが入っていない場合の処理
    return {
      temperature: 30,
      humidity: 80,
      co2: 1000,
      lastUpdate: '2000-01-01T00:00:00Z',
    };
  }

  const data: SensorDataWithTime = JSON.parse(value);

  return data;
};

export const updateSensorData = async (
  KV: KVNamespace,
  id: string,
  data: SensorData
) => {
  const now = new Date();
  const dateStr = now.toISOString().slice(0, -5) + 'Z';

  const dataWithTime: SensorDataWithTime = { ...data, lastUpdate: dateStr };

  const value = JSON.stringify(dataWithTime);
  await KV.put(generateKey(id, 'sensor'), value);

  return dataWithTime;
};

export const getPhoneNumber = async (
  KV: KVNamespace,
  id: string
): Promise<PhoneNumberData | undefined> => {
  const value = await KV.get(generateKey(id, 'phonenumber'));
  if (!value) {
    return undefined;
  }

  const data: PhoneNumberData = JSON.parse(value);

  return data;
};

export const updatePhoneNumber = async (
  KV: KVNamespace,
  id: string,
  phoneNumber: PhoneNumberData
) => {
  const value = JSON.stringify(phoneNumber);
  await KV.put(generateKey(id, 'phonenumber'), value);
};

const getAccessToken = async (clientEmail: string, privateKeyPkcs: string) => {
  const alg = 'RS256';
  const privateKey = await importPKCS8(privateKeyPkcs, alg);
  const jwt = await new SignJWT({
    'urn:example:claim': true,
    scope: 'https://www.googleapis.com/auth/firebase.messaging',
  })
    .setProtectedHeader({ alg })
    .setIssuedAt()
    .setIssuer(clientEmail)
    .setAudience('https://oauth2.googleapis.com/token')
    .setExpirationTime('10m')
    .sign(privateKey);

  // authorization
  const authRes = await fetch('https://accounts.google.com/o/oauth2/token', {
    method: 'POST',
    headers: {
      'content-type': 'application/x-www-form-urlencoded',
    },
    body: `grant_type=urn:ietf:params:oauth:grant-type:jwt-bearer&assertion=${jwt}`,
  });

  const authJson: { access_token?: string } = await authRes.json();
  if (!authJson || !authJson.access_token) throw new Error('auth failed');
  const accessToken = authJson.access_token;

  return accessToken;
};

export const sendAlart = async (
  firebaseKeyFile: string,
  iosToken: string,
  id: string,
  alart: Alart
) => {
  // TODO: アプリへの通知機能を実装
  const notificationMessage = {
    title: '通知',
    body: alart.message,
  };

  // generate jwt
  const firebaseKeyData: {
    client_email: string;
    private_key: string;
  } = JSON.parse(firebaseKeyFile);
  const accessToken = await getAccessToken(
    firebaseKeyData.client_email,
    firebaseKeyData.private_key
  );

  const res = await fetch(
    'https://fcm.googleapis.com/v1/projects/bathcare-fab01/messages:send',
    {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json; UTF-8',
        Authorization: `Bearer ${accessToken}`,
      },
      body: JSON.stringify({
        message: {
          token: iosToken,
          notification: notificationMessage,
        },
      }),
    }
  ).catch((err) => {
    console.error(err);
  });

  if (!res) return { message: 'error' };

  return await res.json();
};

export const getHistory = async (
  KV: KVNamespace,
  id: string
): Promise<History> => {
  const historyStr = await KV.get(generateKey(id, 'history'));
  const history: HistoryEntry[] = historyStr ? JSON.parse(historyStr) : [];
  return { history };
};

export const appendHistory = async (
  KV: KVNamespace,
  id: string,
  newEntry: HistoryEntry
): Promise<History> => {
  const historyStr = await KV.get(generateKey(id, 'history'));
  let history: HistoryEntry[] = historyStr ? JSON.parse(historyStr) : [];

  // Add new entry
  history.push(newEntry);

  // If history is lnger than 100, remove old entries
  const length = history.length;
  if (length > 100) {
    history = history.slice(length - 100);
  }

  // Save history
  const newHistoryStr = JSON.stringify(history);
  await KV.put(generateKey(id, 'history'), newHistoryStr);

  return { history };
};

export const dangerousDeleteAll = async (KV: KVNamespace) => {
  const keys = await KV.list();
  for (const key of keys.keys) {
    await KV.delete(key.name);
  }
};

export const dangerousDeleteById = async (KV: KVNamespace, id: string) => {
  const keyKinds: KeyKind[] = ['status', 'sensor', 'phonenumber', 'history'];
  const keys = keyKinds.map((kind) => generateKey(id, kind));

  await Promise.all(keys.map((key) => KV.delete(key)));
};
