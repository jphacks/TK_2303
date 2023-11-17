import { drizzle } from 'drizzle-orm/d1';
import {
  DeviceStatus,
  History,
  HistoryEntry,
  SensorData,
  SensorDataWithTime,
} from '../types';

import { generateKey, statusToHistoryEntry } from '../util';
import { getDevice, updateDevice } from '../db/crud';

export const getStatus = async (
  DB: D1Database,
  id: string
): Promise<DeviceStatus> => {
  const db = drizzle(DB);
  const device = await getDevice(db, id);
  if (!device) {
    return {
      status: 'unknown',
    };
  }

  const status: DeviceStatus = {
    status: device.status,
  };
  return status;
};

export const updateStatus = async (
  DB: D1Database,
  KV: KVNamespace,
  id: string,
  status: DeviceStatus
) => {
  // Get old status
  const oldStatus = await getStatus(DB, id);

  // Update status
  const db = drizzle(DB);
  await updateDevice(db, { id, status: status.status });

  // If status is changed, update history
  if (status.status !== 'unknown' && oldStatus.status !== status.status) {
    const historyEntry = statusToHistoryEntry(status);
    await appendHistory(KV, id, historyEntry);
  }
};

export const getSensorData = async (
  DB: D1Database,
  id: string
): Promise<SensorDataWithTime> => {
  const db = drizzle(DB);
  const device = await getDevice(db, id);
  if (
    !device ||
    !device.temperature ||
    !device.humidity ||
    !device.pressure ||
    !device.lastUpdate
  ) {
    // TODO: データが入っていない場合の処理
    return {
      temperature: 30,
      humidity: 80,
      pressure: 1000,
      lastUpdate: '2000-01-01T00:00:00Z',
    };
  }

  const data: SensorDataWithTime = {
    temperature: device.temperature,
    humidity: device.humidity,
    pressure: device.pressure,
    lastUpdate: device.lastUpdate.toISOString(),
  };

  return data;
};

export const updateSensorData = async (
  DB: D1Database,
  id: string,
  data: SensorData
): Promise<SensorDataWithTime> => {
  const db = drizzle(DB);
  const dataWithTime = { id, ...data, lastUpdate: new Date() };

  await updateDevice(db, dataWithTime);

  return { ...data, lastUpdate: dataWithTime.lastUpdate.toISOString() };
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
