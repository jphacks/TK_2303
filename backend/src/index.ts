import { Hono } from 'hono';

import { Alart, DeviceStatus, SensorData, SensorDataWithTime } from './types';
import { Bindings } from './bindings';
import * as model from './model';

const app = new Hono<{ Bindings: Bindings }>();

app.get('/status/:id', async (c) => {
  const deviceId = c.req.param('id');

  const status = await model.getStatus(c.env.DEVICE_STATUSES, deviceId);

  return c.json<DeviceStatus>(status);
});

app.post('/status/:id', async (c) => {
  const deviceId = c.req.param('id');
  const status = await c.req.json<DeviceStatus>();

  await model.updateStatus(c.env.DEVICE_STATUSES, deviceId, status);

  return c.json<DeviceStatus>(status);
});

app.get('/sensors/:id', async (c) => {
  const deviceId = c.req.param('id');

  const sensorData = await model.getSensorData(c.env.DEVICE_STATUSES, deviceId);

  return c.json<SensorDataWithTime>(sensorData);
});

app.post('/sensors/:id', async (c) => {
  const deviceId = c.req.param('id');
  const sensorData = await c.req.json<SensorData>();

  const dataWithTime = await model.updateSensorData(
    c.env.DEVICE_STATUSES,
    deviceId,
    sensorData
  );

  return c.json<SensorDataWithTime>(dataWithTime);
});

app.post('/alart/:id', async (c) => {
  const deviceId = c.req.param('id');
  const alart = await c.req.json<Alart>();

  model.sendAlart(c.env.DEVICE_STATUSES, deviceId, alart);

  return c.json<Alart>(alart);
});

export default app;
