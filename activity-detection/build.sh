#!/bin/bash

set -eux

rm -rf libs
pip install -r requirements.txt -t ./libs
