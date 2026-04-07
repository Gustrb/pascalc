#!/bin/sh
set -e

failed=0

for test in ./dist/tests/*; do
  start=$(date +%s%N)
  if "$test"; then
    end=$(date +%s%N)
    elapsed=$(( (end - start) / 1000000 ))
    echo "[SUITE]: $test passed (${elapsed}ms)"
  else
    end=$(date +%s%N)
    elapsed=$(( (end - start) / 1000000 ))
    echo "[SUITE]: $test failed (${elapsed}ms)"
    failed=1
  fi
done

exit $failed
