import { z } from 'zod';

export type Device = {
  id: string;
  status: 'in' | 'out' | 'danger' | 'unknown';
  name?: string;
  registerDate: Date;
  temperature?: number;
  humidity?: number;
  pressure?: number;
  lastUpdate?: Date;
  phoneNumber?: string;
  alartToken?: string;
};

export type DeviceStatus = {
  status: 'in' | 'out' | 'danger' | 'unknown';
};

export const DeviceStatusSchema = z.object({
  status: z.enum(['in', 'out', 'danger', 'unknown']),
});

export type Alart = {
  message: string;
};

export const AlartSchema = z.object({
  message: z.string(),
});

export type SensorData = {
  temperature: number;
  humidity: number;
  pressure: number;
};

export const SensorDataSchema = z.object({
  temperature: z.number().min(-100).max(100),
  humidity: z.number().min(0).max(100),
  pressure: z.number().min(0).max(10000),
});

export type SensorDataWithTime = SensorData & {
  lastUpdate: string;
};

export const SensorDataWithTimeSchema = SensorDataSchema.extend({
  lastUpdate: z.string(),
});

export const AlartTokenSchema = z.object({
  token: z.string(),
});

export type PhoneNumberData = {
  number: string;
};

export const PhoneNumberDataSchema = z.object({
  number: z.string(),
});

export type CheckSafeResponse = {
  safe: boolean;
  transcription: string;
  chatBotResponse: string;
  transcriptionDuration: number;
  chatDuration: number;
};

export type HistoryEntry = {
  date: string;
  action: 'goIntoBath' | 'goOutOfBath' | 'becomeDanger';
};

export const HistoryEntrySchema = z.object({
  date: z.string(),
  action: z.enum(['goIntoBath', 'goOutOfBath', 'becomeDanger']),
});

export type History = { history: HistoryEntry[] };

export type Firmware = {
  name: string;
  version: number;
  release_date: string;
  download_url: string;
};
