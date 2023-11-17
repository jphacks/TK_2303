import { Hono } from 'hono';
import { bearerAuth } from 'hono/bearer-auth';

import { authMiddleware } from '../auth';
import { Bindings } from '../bindings';
import * as model from '../models/device';

// Under `/device`
const deviceRoute = new Hono<{ Bindings: Bindings }>();

deviceRoute.post(
  '/register',
  // bearerAuth({token:"secret"}),
  async (c, next) => {
    // Protect this route with a bearer token
    await bearerAuth({ token: c.env.REGISTER_API_TOKEN })(c, next);
  },
  async (c) => {
    const token = await model.registerDevice(c.env.DB);

    return c.json({ token });
  }
);

deviceRoute.get('/verify', authMiddleware, async (c) => {
  return c.json({ message: 'OK' });
});

export default deviceRoute;
