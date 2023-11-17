import { Hono } from 'hono';

import { Bindings } from './bindings';
import bathRoute from './routes/bath';
import miscRoute from './routes/misc';
import deviceRoute from './routes/device';
import firmwareRoute from './routes/firmware';

const app = new Hono<{ Bindings: Bindings }>();

app.route('/bath', bathRoute);

app.route('/misc', miscRoute);

app.route('/device', deviceRoute);

app.route('/firmware', firmwareRoute);

export default app;
