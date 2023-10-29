import { DeviceStatus, HistoryEntry } from './types';

export const getCurrentTimeStr = () => {
  const now = new Date();
  return now.toISOString().slice(0, -5) + 'Z';
};

export const statusToHistoryEntry = (status: DeviceStatus): HistoryEntry => {
  let action: 'goIntoBath' | 'goOutOfBath' | 'becomeDanger';
  switch (status.status) {
    case 'in':
      action = 'goIntoBath';
      break;
    case 'out':
      action = 'goOutOfBath';
      break;
    case 'danger':
      action = 'becomeDanger';
      break;
    case 'unknown':
      throw new Error('Unknown status');
  }

  return {
    action,
    date: getCurrentTimeStr(),
  };
};
