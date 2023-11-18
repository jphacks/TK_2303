import sys
import os

sys.path.append(os.path.join(os.getcwd(), os.path.dirname(__file__), "./libs"))

import json
import webrtcvad
from pydub import AudioSegment
import io
import base64


def lambda_handler(event, context):
    # TODO implement
    wav_base64 = json.loads(event["body"])["wav"]

    # decode from base64 string
    wav_bytes = base64.b64decode(wav_base64)

    # run detection
    results = run_detection(wav_bytes)

    return {"statusCode": 200, "body": json.dumps({"results": results})}


def run_detection(wav_bytes, frame_duration=10) -> list:
    vad = webrtcvad.Vad()
    sample_rate = 16000

    with io.BytesIO(wav_bytes) as wav_file:
        audio = AudioSegment.from_file(wav_file)

    # モノラルに変換
    audio = audio.set_channels(1)

    # サンプルレートを16000Hzに変換
    audio = audio.set_frame_rate(sample_rate)

    # 16ビット整数のPCM形式に変換
    audio = audio.set_sample_width(2)

    data = audio.raw_data

    nbytes_per_frame = sample_rate * frame_duration * 2 // 1000

    results = []
    for start in range(0, len(data) - nbytes_per_frame, nbytes_per_frame):
        results.append(
            vad.is_speech(data[start : start + nbytes_per_frame], sample_rate)
        )

    return results
