#!/bin/bash

set -eux

basefile="$1"

tmpwav="tmp.wav"
duration=8
for i in {0..10}; do
    trimedfile="samples/sample_$i.wav"
    # trim 8s
    sox "$basefile" "$tmpwav" trim $(( i * duration )) $(( (i + 1) * duration ))
    # resample to 8000Hz
    sox "$tmpwav" -r 16000 "$trimedfile"
    python3 tools/to_json.py "$trimedfile" "./samples/sample_$i.json"
done

rm $tmpwav
