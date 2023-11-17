import { MiddlewareHandler } from 'hono';
import { getBearerToken } from './util';
import { Bindings, AuthVariables } from './bindings';
import { verifyDevice } from './models/device';

export const authMiddleware: MiddlewareHandler<{
  Bindings: Bindings;
  Variables: AuthVariables;
}> = async (c, next) => {
  const token = getBearerToken(c.req.header('Authorization'));
  if (!token) {
    c.status(401);
    return c.json({ message: 'Unauthorized' });
  }

  const hashed = await verifyDevice(c.env.DB, token);
  if (!hashed) {
    return c.json({ message: 'Unauthorized' }, 401);
  }

  // Set device token to header
  c.set('token', token);
  c.set('hashed', hashed);
  await next();
};
