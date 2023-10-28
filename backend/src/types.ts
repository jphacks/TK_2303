export type DeviceStatus = {
  status: 'in' | 'out' | 'danger' | 'unknown';
};

export type Alart = {
  message: string;
};

export type SensorData = {
  temperature: number;
  humidity: number;
  co2: number;
};

export type SensorDataWithTime = SensorData & {
  lastUpdate: string;
};
