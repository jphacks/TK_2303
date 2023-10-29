import { Hono } from 'hono';

import {
  Alart,
  AlartSchema,
  DeviceStatus,
  DeviceStatusSchema,
  PhoneNumberData,
  PhoneNumberDataSchema,
  SensorDataSchema,
  SensorDataWithTime,
} from './types';
import { Bindings } from './bindings';
import * as model from './model';
import { ZodError } from 'zod';

const app = new Hono<{ Bindings: Bindings }>();

app.get('/status/:id', async (c) => {
  const deviceId = c.req.param('id');

  const status = await model.getStatus(c.env.DEVICE_STATUSES, deviceId);

  return c.json<DeviceStatus>(status);
});

app.post('/status/:id', async (c) => {
  const deviceId = c.req.param('id');
  try {
    const status = DeviceStatusSchema.parse(await c.req.json<DeviceStatus>());

    await model.updateStatus(c.env.DEVICE_STATUSES, deviceId, status);

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

app.get('/sensors/:id', async (c) => {
  const deviceId = c.req.param('id');

  const sensorData = await model.getSensorData(c.env.DEVICE_STATUSES, deviceId);

  return c.json<SensorDataWithTime>(sensorData);
});

app.post('/sensors/:id', async (c) => {
  const deviceId = c.req.param('id');
  try {
    const sensorData = SensorDataSchema.parse(await c.req.json());

    const dataWithTime = await model.updateSensorData(
      c.env.DEVICE_STATUSES,
      deviceId,
      sensorData
    );

    return c.json<SensorDataWithTime>(dataWithTime);
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

app.post('/alart/:id', async (c) => {
  const deviceId = c.req.param('id');
  try {
    const alart = AlartSchema.parse(await c.req.json<Alart>());

    await model.sendAlart(
      c.env.FIREBASE_KEY_FILE,
      c.env.IOS_TOKEN,
      deviceId,
      alart
    );

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

app.get('/phonenumber/:id', async (c) => {
  const deviceId = c.req.param('id');

  const phonenumber = await model.getPhoneNumber(
    c.env.DEVICE_STATUSES,
    deviceId
  );

  if (!phonenumber) {
    c.status(404);
    return c.json({ message: 'Not Found' });
  }

  return c.json<PhoneNumberData>(phonenumber);
});

app.post('/phonenumber/:id', async (c) => {
  const deviceId = c.req.param('id');
  try {
    const phonenumber = PhoneNumberDataSchema.parse(
      await c.req.json<PhoneNumberData>()
    );

    await model.updatePhoneNumber(c.env.DEVICE_STATUSES, deviceId, phonenumber);

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

app.get('/history/:id', async (c) => {
  const deviceId = c.req.param('id');

  const history = await model.getHistory(c.env.DEVICE_STATUSES, deviceId);

  return c.json<History>(history);
});

// Delete all data
app.post('/dangerous/deleteAll', async (c) => {
  await model.dangerousDeleteAll(c.env.DEVICE_STATUSES);
  return c.json({ message: 'OK' });
});

// Delete data by id
app.post('/dangerous/delete/:id', async (c) => {
  const deviceId = c.req.param('id');
  await model.dangerousDeleteById(c.env.DEVICE_STATUSES, deviceId);
  return c.json({ message: 'OK' });
});

export default app;
