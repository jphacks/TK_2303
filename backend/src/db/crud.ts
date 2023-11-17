import { DrizzleD1Database } from 'drizzle-orm/d1';

import { devices } from './schema';
import { Device } from '../types';
import { eq } from 'drizzle-orm';

export const getDevice = async (
  db: DrizzleD1Database,
  id: string
): Promise<Device | undefined> => {
  const result = await db.select().from(devices).where(eq(devices.id, id));
  const device: Device = {
    id: result[0].id,
    name: result[0].name ?? undefined,
    registerDate: result[0].registerDate,
    status: result[0].status,
    temperature: result[0].temperature ?? undefined,
    humidity: result[0].humidity ?? undefined,
    pressure: result[0].pressure ?? undefined,
    lastUpdate: result[0].lastUpdate ?? undefined,
    phoneNumber: result[0].phoneNumber ?? undefined,
    alartToken: result[0].alartToken ?? undefined,
  };
  return result.length > 0 ? device : undefined;
};

export const updateDevice = async (
  db: DrizzleD1Database,
  device: { id: string } & Partial<Device>
) => {
  const updateDevice = {
    ...device,
    id: undefined,
  };
  await db.update(devices).set(updateDevice).where(eq(devices.id, device.id));
};
