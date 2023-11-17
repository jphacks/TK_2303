import { drizzle } from 'drizzle-orm/d1';
import { devices } from '../db/schema';
import { generateKey, generateRandomToken, generateTokenHash } from '../util';
import { eq } from 'drizzle-orm';

export const registerDevice = async (DB: D1Database): Promise<string> => {
  const token = generateRandomToken();
  const hash = generateTokenHash(token, 'salt');
  const registerDate = new Date();

  const db = drizzle(DB);
  await db.insert(devices).values({
    id: hash,
    registerDate,
  });

  return token;
};

export const verifyDevice = async (
  DB: D1Database,
  token: string | undefined
): Promise<string | undefined> => {
  if (!token) return undefined;
  const hash = generateTokenHash(token, 'salt');

  const db = drizzle(DB);
  const result = await db.select().from(devices).where(eq(devices.id, hash));

  return result.length > 0 ? hash : undefined;
};
