import sys
import os

sys.path.append(os.path.join(os.getcwd(), os.path.dirname(__file__), "./libs"))

import json
import webrtcvad
from pydub import AudioSegment
import io
import base64
import urllib.request
import librosa
import numpy as np


def lambda_handler(event, context):
    wav_base64 = event.get("wav")
    if wav_base64 == None:
        wav_base64 = json.loads(event["body"])["wav"]

    # decode from base64 string
    wav_bytes = base64.b64decode(wav_base64)

    # run detection
    results = run_detection(wav_bytes)

    audio_urls = event.get("audio_urls")
    if audio_urls == None:
        audio_urls = json.loads(event["body"])["audio_urls"]

    mfcc_dists = comp_mfccs(audio_urls)

    return {
        "statusCode": 200,
        "body": json.dumps({"results": results, "mfcc_dists": mfcc_dists}),
    }


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


def comp_mfccs(audio_urls: list[str]):
    mfccs_list = []
    for url in audio_urls:
        mfccs = get_mfccs(url)
        mfccs_list.append(mfccs)

    num_mfccs = len(mfccs_list)

    means = [np.mean(mfccs_list[i], axis=1) for i in range(num_mfccs)]

    dists = []
    for i in range(num_mfccs):
        dists_tmp = []
        for j in range(num_mfccs):
            dist = np.linalg.norm(means[i] - means[j]) if i != j else -1
            dists_tmp.append(float(dist))
        dists.append(dists_tmp)

    return dists

def get_mfccs(url: str):
    req = urllib.request.Request(url, headers={"User-Agent": "urllib"})
    with urllib.request.urlopen(req) as f:
        wav_bytes = f.read()
        with io.BytesIO(wav_bytes) as wav_file:
            audio, sr = librosa.load(wav_file, sr=8000)

    mfccs = librosa.feature.mfcc(y=audio, sr=sr, n_mfcc=32)

    return mfccs
