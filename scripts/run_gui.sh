#!/bin/bash
cd "$(dirname "$0")/.."
make gui && ./build/morphology_gui
