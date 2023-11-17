import OpenAI from 'openai';
import { Ai } from '@cloudflare/ai';
import { AiSpeechRecognitionOutput } from '@cloudflare/ai/dist/tasks/speech-recognition';
import { drizzle } from 'drizzle-orm/d1';
import { SignJWT, importPKCS8 } from 'jose';

import { Alart, CheckSafeResponse, PhoneNumberData } from '../types';
import { getDevice, updateDevice } from '../db/crud';
import { retryFetch } from '../util';

export const getPhoneNumber = async (
  DB: D1Database,
  id: string
): Promise<PhoneNumberData | undefined> => {
  const db = drizzle(DB);
  const device = await getDevice(db, id);
  if (!device || !device.phoneNumber) {
    return undefined;
  }

  const data: PhoneNumberData = {
    number: device.phoneNumber,
  };

  return data;
};

export const updatePhoneNumber = async (
  DB: D1Database,
  id: string,
  phoneNumber: PhoneNumberData
) => {
  const db = drizzle(DB);
  await updateDevice(db, { id, phoneNumber: phoneNumber.number });
};

const getAccessToken = async (clientEmail: string, privateKeyPkcs: string) => {
  const alg = 'RS256';
  const privateKey = await importPKCS8(privateKeyPkcs, alg);
  const jwt = await new SignJWT({
    'urn:example:claim': true,
    scope: 'https://www.googleapis.com/auth/firebase.messaging',
  })
    .setProtectedHeader({ alg })
    .setIssuedAt()
    .setIssuer(clientEmail)
    .setAudience('https://oauth2.googleapis.com/token')
    .setExpirationTime('10m')
    .sign(privateKey);

  // authorization
  const authRes = await fetch('https://accounts.google.com/o/oauth2/token', {
    method: 'POST',
    headers: {
      'content-type': 'application/x-www-form-urlencoded',
    },
    body: `grant_type=urn:ietf:params:oauth:grant-type:jwt-bearer&assertion=${jwt}`,
  });

  const authJson: { access_token?: string } = await authRes.json();
  if (!authJson || !authJson.access_token) throw new Error('auth failed');
  const accessToken = authJson.access_token;

  return accessToken;
};

export const getAlartToken = async (
  DB: D1Database,
  id: string
): Promise<string | undefined> => {
  const db = drizzle(DB);
  const device = await getDevice(db, id);
  if (!device || !device.alartToken) {
    return undefined;
  }

  return device.alartToken;
};

export const updateAlartToken = async (
  DB: D1Database,
  id: string,
  token: string
) => {
  const db = drizzle(DB);
  await updateDevice(db, { id, alartToken: token });
};

export const sendAlart = async (
  DB: D1Database,
  firebaseKeyFile: string,
  id: string,
  alart: Alart
) => {
  const db = drizzle(DB);
  const device = await getDevice(db, id);
  if (!device || !device.alartToken) return { message: 'error' };

  const iosToken = device.alartToken;

  // TODO: アプリへの通知機能を実装
  const notificationMessage = {
    title: '通知',
    body: alart.message,
  };

  // generate jwt
  const firebaseKeyData: {
    client_email: string;
    private_key: string;
  } = JSON.parse(firebaseKeyFile);
  const accessToken = await getAccessToken(
    firebaseKeyData.client_email,
    firebaseKeyData.private_key
  );

  const res = await fetch(
    'https://fcm.googleapis.com/v1/projects/bathcare-fab01/messages:send',
    {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json; UTF-8',
        Authorization: `Bearer ${accessToken}`,
      },
      body: JSON.stringify({
        message: {
          token: iosToken,
          notification: notificationMessage,
        },
      }),
    }
  ).catch((err) => {
    console.error(err);
  });

  if (!res) return { message: 'error' };

  return await res.json();
};

export const checkSafe = async (
  OPENAI_API_KEY: string,
  OPENAI_ORG_ID: string,
  audio: Uint8Array,
  AI: any
): Promise<CheckSafeResponse> => {
  const openai = new OpenAI({
    apiKey: OPENAI_API_KEY,
    organization: OPENAI_ORG_ID,
  });

  const ai = new Ai(AI);

  let perfStart = performance.now();
  const transcription = (await ai.run('@cf/openai/whisper', {
    audio: [...audio],
  })) as AiSpeechRecognitionOutput;
  const transcriptionDuration = (performance.now() - perfStart) / 1000; // msec -> sec

  perfStart = performance.now();

  const chatResponse = await retryFetch(
    async () => {
      const res = await openai.chat.completions.create({
        model: 'gpt-3.5-turbo-1106',
        messages: [
          {
            role: 'system',
            content: `You are an agent tasked with analyzing input statements to determine if the speaker is currently safe. The statements you receive are made in response to the question "A danger has been detected. Are you okay?" and are in Japanese. Please respond only with "yes" or "no" to the question of safety, and avoid any additional output. If there is no spoken word provided, please output "no". Also, if the user's response is clearly inappropriate or nonsensical, please output "no".`,
          },
          {
            role: 'user',
            content: ` ${transcription.text}`,
          },
        ],
      });
      return res;
    },
    {
      retryCount: 5, // 5 times
      retryTimeout: 4, // 4 sec interval
    }
  );

  const chatDuration = (performance.now() - perfStart) / 1000; // msec -> sec

  // If the content is empty, it means that the user did not respond.
  // const chatBotResponse = chatResponse.response || 'yes';
  const chatBotResponse = chatResponse.choices[0].message.content || 'yes';
  return {
    safe: chatBotResponse?.toLowerCase().includes('yes'),
    transcription: transcription.text || '',
    chatBotResponse,
    transcriptionDuration,
    chatDuration,
  };
};
