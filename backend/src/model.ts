import { generateKey, KeyKind } from './util';

export const dangerousDeleteAll = async (KV: KVNamespace) => {
  const keys = await KV.list();
  for (const key of keys.keys) {
    await KV.delete(key.name);
  }
};

export const dangerousDeleteById = async (KV: KVNamespace, id: string) => {
  const keyKinds: KeyKind[] = ['status', 'sensor', 'phonenumber', 'history'];
  const keys = keyKinds.map((kind) => generateKey(id, kind));

  await Promise.all(keys.map((key) => KV.delete(key)));
};
