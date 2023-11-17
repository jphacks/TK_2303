import { Hono } from 'hono';

import { Firmware } from '../types';
import { Bindings } from '../bindings';
import { authMiddleware } from '../auth';

// Under `/firmware`
const firmwareRoute = new Hono<{ Bindings: Bindings }>();

firmwareRoute.get('/versions', authMiddleware, async (c) => {
  const boardRevision = c.req.query('board_revision') || 'all';
  return c.json<{ versions: Firmware[] }>({
    versions: [
      {
        version: 1,
        name: 'test',
        download_url: 'https://bathcare.tsubame.workers.dev/firmware.bin',
        release_date: '2023-11-12',
      },
    ],
  });
});

firmwareRoute.get('/versions/latest', authMiddleware, async (c) => {
  const boardRevision = c.req.query('board_revision') || 'all';
  return c.json<Firmware>({
    version: 1,
    name: 'test',
    download_url: `${c.env.FIRMWARE_DOWNLOAD_SITE}/firmware-update-test.bin`,
    release_date: '2023-11-12',
  });
});

// Download firmware (no auth)
firmwareRoute.get('/download/:filename', async (c) => {
  c.status(404);
  return c.json({ message: 'Not Found' });
});

export default firmwareRoute;
