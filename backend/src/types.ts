import { z } from 'zod';

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
  co2: number;
};

export const SensorDataSchema = z.object({
  temperature: z.number().min(-100).max(100),
  humidity: z.number().min(0).max(100),
  co2: z.number().min(0).max(10000),
});

export type SensorDataWithTime = SensorData & {
  lastUpdate: string;
};

export const SensorDataWithTimeSchema = SensorDataSchema.extend({
  lastUpdate: z.string(),
});

export type PhoneNumberData = {
  number: string;
};

export const PhoneNumberDataSchema = z.object({
  number: z.string(),
});
