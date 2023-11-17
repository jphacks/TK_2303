import {
  sqliteTable,
  text,
  real,
  integer,
  uniqueIndex,
} from 'drizzle-orm/sqlite-core';

export const devices = sqliteTable(
  'devices',
  {
    id: text('id').primaryKey().unique().notNull(),
    name: text('name'),
    registerDate: integer('registerDate', { mode: 'timestamp' }).notNull(),
    status: text('status', { enum: ['in', 'out', 'danger', 'unknown'] })
      .default('unknown')
      .notNull(),
    temperature: real('temperature'),
    humidity: real('humidity'),
    pressure: real('pressure'),
    lastUpdate: integer('lastUpdate', { mode: 'timestamp' }),
    phoneNumber: text('phoneNumber'),
    alartToken: text('alartToken'),
  },
  (table) => ({
    idx: uniqueIndex('idx').on(table.id),
  })
);
