import nodeCrypto from 'node:crypto';
import { DeviceStatus, HistoryEntry } from './types';

export const getCurrentTimeStr = () => {
  const now = new Date();
  return now.toISOString().slice(0, -5) + 'Z';
};

export const statusToHistoryEntry = (status: DeviceStatus): HistoryEntry => {
  let action: 'goIntoBath' | 'goOutOfBath' | 'becomeDanger';
  switch (status.status) {
    case 'in':
      action = 'goIntoBath';
      break;
    case 'out':
      action = 'goOutOfBath';
      break;
    case 'danger':
      action = 'becomeDanger';
      break;
    case 'unknown':
      throw new Error('Unknown status');
  }

  return {
    action,
    date: getCurrentTimeStr(),
  };
};

export const generateRandomToken = () => {
  // To generate a 256-bit token, we need 32 bytes.
  const length = 32;

  const token: string = nodeCrypto
    .randomBytes(length)
    .toString('base64')
    .replace(/\+/g, '-')
    .replace(/\//g, '_')
    .replace(/=+$/, '');

  return token;
};

/**
 * Generate a hash of the token.
 * @param token
 */
export const generateTokenHash = (token: string, salt: string) => {
  const iteration = 4096;
  const keyLength = 32;
  const digest = 'sha256';

  const hashed: string = nodeCrypto
    .pbkdf2Sync(token, salt, iteration, keyLength, digest)
    .toString('base64');

  return hashed;
};

export const getBearerToken = (authHeader: string | undefined) => {
  if (!authHeader) {
    return undefined;
  }
  const [type, token] = authHeader.split(' ');
  if (type !== 'Bearer') {
    return undefined;
  }
  return token;
};

// KV key generation
const KEY_PREFIX = 'dev:v0';
export type KeyKind = 'status' | 'sensor' | 'phonenumber' | 'history' | 'token';
export const generateKey = (id: string, kind: KeyKind) => {
  return `${KEY_PREFIX}:${kind}:${id}`;
};

/**
 *
 * @param targetFunc fetch function
 * @param opt retry options
 * @returns
 */
export const retryFetch = <T>(
  targetFunc: () => Promise<T>,
  opt?: {
    retryCount?: number;
    retryTimeout?: number; // seconds
  }
): Promise<T> => {
  return new Promise((resolve, reject) => {
    const retryCount = opt?.retryCount || 3;
    const retryTimeout = opt?.retryTimeout || 10;
    let count = 0;

    let intervalId: NodeJS.Timeout | undefined = undefined;

    const clearAndResolve = (res: T) => {
      if (intervalId) clearInterval(intervalId);
      resolve(res);
    };

    const clearAndReject = (err: Error) => {
      if (intervalId) clearInterval(intervalId);
      reject(err);
    };

    const intervalFunc = () => {
      count++;
      if (count > retryCount) {
        clearAndReject(new Error('Retry count exceeded'));
      }

      // Repeat unit
      const repeatFunc = () => {
        targetFunc()
          .then((res) => {
            clearAndResolve(res);
          })
          .catch(() => {
            // If error catched, clear interval and start another interval.
            if (intervalId) clearInterval(intervalId);
            intervalFunc();
          });
      };

      // Because the first interval start after retryTimeout seconds,
      // we need to call repeatFunc() once in outside of setInterval().
      repeatFunc();

      // interval start
      intervalId = setInterval(() => {
        count++;
        if (count > retryCount) {
          clearAndReject(new Error('Retry count exceeded'));
        }

        repeatFunc();
      }, retryTimeout * 1000);
    };

    // Start the first interval
    intervalFunc();
  });
};
