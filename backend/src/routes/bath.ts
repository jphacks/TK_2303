import { Hono } from 'hono';
import { ZodError } from 'zod';

import { AuthVariables, Bindings } from '../bindings';
import {
  DeviceStatus,
  DeviceStatusSchema,
  SensorDataWithTime,
  SensorDataSchema,
  History,
} from '../types';
import * as model from '../models/bath';
import { authMiddleware } from '../auth';

// Under `/bath`
const bathRoute = new Hono<{ Bindings: Bindings; Variables: AuthVariables }>();

// All routes under `/bath` require authentication
bathRoute.use('*', authMiddleware);

bathRoute.get('/status', async (c) => {
  const deviceId = c.get('hashed');

  const status = await model.getStatus(c.env.DB, deviceId);

  return c.json<DeviceStatus>(status);
});

bathRoute.post('/status', async (c) => {
  const deviceId = c.get('hashed');
  try {
    const status = DeviceStatusSchema.parse(await c.req.json<DeviceStatus>());

    await model.updateStatus(c.env.DB, c.env.DEVICE_STATUSES, deviceId, status);

    return c.json<DeviceStatus>(status);
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

bathRoute.get('/sensors', async (c) => {
  const deviceId = c.get('hashed');

  const sensorData = await model.getSensorData(c.env.DB, deviceId);

  return c.json<SensorDataWithTime>(sensorData);
});

bathRoute.post('/sensors', async (c) => {
  const deviceId = c.get('hashed');
  try {
    const sensorData = SensorDataSchema.parse(await c.req.json());

    const dataWithTime = await model.updateSensorData(
      c.env.DB,
      deviceId,
      sensorData
    );

    return c.json<SensorDataWithTime>({
      ...dataWithTime,
    });
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

bathRoute.get('/history', async (c) => {
  const deviceId = c.get('hashed');

  const history = await model.getHistory(c.env.DEVICE_STATUSES, deviceId);

  return c.json<History>(history);
});

export default bathRoute;
