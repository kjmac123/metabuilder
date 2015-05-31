#!/bin/sh
set -e

rsync -av "data/" "projects/android/assets/data/"
