import { Hono } from 'hono';
import { ZodError } from 'zod';

import { AuthVariables, Bindings } from '../bindings';
import {
  Alart,
  AlartSchema,
  AlartTokenSchema,
  CheckSafeResponse,
  PhoneNumberData,
  PhoneNumberDataSchema,
} from '../types';
import * as model from '../models/misc';
import { authMiddleware } from '../auth';
import { generateRandomToken } from '../util';

// Under `/misc`
const miscRoute = new Hono<{ Bindings: Bindings; Variables: AuthVariables }>();

miscRoute.use('*', authMiddleware);

miscRoute.post('/alart', async (c) => {
  const deviceId = c.get('hashed');
  try {
    const alart = AlartSchema.parse(await c.req.json<Alart>());

    await model.sendAlart(c.env.DB, c.env.FIREBASE_KEY_FILE, deviceId, alart);

    return c.json<Alart>(alart);
  } catch (err) {
    if (err instanceof ZodError) {
      // validation error
      c.status(400);
      return c.json({ message: 'Bad Request' });
    } else {
      // unknown error
      console.error(err);
      c.status(500);
      return c.json({ message: 'Internal Server Error' });
    }
  }
});

miscRoute.get('/alart/token', async (c) => {
  const deviceId = c.get('hashed');

  const token = await model.getAlartToken(c.env.DB, deviceId);

  if (!token) {
    c.status(404);
    return c.json({ message: 'Not Found' });
  }

  return c.json<{ token: string }>({ token });
});

miscRoute.post('/alart/token', async (c) => {
  const deviceId = c.get('hashed');
  try {
    const { token } = AlartTokenSchema.parse(await c.req.json());

    await model.updateAlartToken(c.env.DB, deviceId, token);

    return c.json<{ token: string }>({ token });
  } catch (err) {
    if (err instanceof ZodError) {
      // validation error
      return c.json({ message: 'Bad Request' }, 400);
    } else {
      // unknown error
      console.error(err);
      c.status(500);
      return c.json({ message: 'Internal Server Error' });
    }
  }
});

miscRoute.get('/phonenumber', async (c) => {
  const deviceId = c.get('hashed');

  const phonenumber = await model.getPhoneNumber(c.env.DB, deviceId);

  if (!phonenumber) {
    c.status(404);
    return c.json({ message: 'Not Found' });
  }

  return c.json<PhoneNumberData>(phonenumber);
});

miscRoute.post('/phonenumber', async (c) => {
  const deviceId = c.get('hashed');
  try {
    const phonenumber = PhoneNumberDataSchema.parse(
      await c.req.json<PhoneNumberData>()
    );

    await model.updatePhoneNumber(c.env.DB, deviceId, phonenumber);

    return c.json<PhoneNumberData>(phonenumber);
  } catch (err) {
    if (err instanceof ZodError) {
      // validation error
      c.status(400);
      return c.json({ message: 'Bad Request' });
    } else {
      // unknown error
      console.error(err);
      c.status(500);
      return c.json({ message: 'Internal Server Error' });
    }
  }
});

// OpenAI APIを使って、音声から安全かどうかを判定する
miscRoute.post(
  '/check_safe',
  async (c, next) => {
    if (c.req.header('Content-Type') !== 'audio/wav') {
      return c.json(
        {
          message: 'Please set `Content-Type` header `audio/wav`',
        },
        400
      );
    }
    await next();
  },
  async (c) => {
    const audio = await c.req.arrayBuffer();

    const safe = await model.checkSafe(
      c.env.OPENAI_API_KEY,
      c.env.OPENAI_ORG_ID,
      new Uint8Array(audio),
      c.env.AI
    );

    const filename = `${generateRandomToken()}.wav`;
    const filepath = `https://bathcare-audio-bucket.so298.net/${filename}`;
    await c.env.AUDIO_BUCKET.put(filename, audio, {
      httpMetadata: {
        contentType: 'audio/wav',
      },
    });

    return c.json<CheckSafeResponse | { filepath: string }>({
      ...safe,
      filepath,
    });
  }
);

export default miscRoute;
