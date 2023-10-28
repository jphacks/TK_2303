import {
  Alart,
  DeviceStatus,
  PhoneNumberData,
  SensorData,
  SensorDataWithTime,
} from './types';

const KEY_PREFIX = 'dev:v0';
type KeyKind = 'status' | 'sensor' | 'phonenumber';
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
  const value = JSON.stringify(status);
  await KV.put(generateKey(id, 'status'), value);
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

export const sendAlart = async (KV: KVNamespace, id: string, alart: Alart) => {
  // TODO: アプリへの通知機能を実装
};
