import json
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("input", type=str)
parser.add_argument("output", type=str)

args = parser.parse_args()

with open(args.input, "rb") as f:
    frames = f.read()

# encode to base64
import base64

encoded = base64.b64encode(frames).decode("ascii")
obj = {"wav": encoded}

with open(args.output, "w") as f:
    json.dump(obj, f)
