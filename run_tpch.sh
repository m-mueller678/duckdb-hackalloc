#!/bin/bash

: "${HACK_ALLOC:?Environment variable HACK_ALLOC is not set. Set to size of alloc pool in GiB}"
SF=${SF:-1}

run_cmd() {
  "$@"
  status=$?

  if [[ $status -ge 128 ]]; then
    signal=$((status - 128))
    case $signal in
      2)  echo "Command '$*' was interrupted (SIGINT). Exiting..."; exit 130 ;;
      3)  echo "Command '$*' received SIGQUIT. Exiting..."; exit 131 ;;
      9)  echo "Command '$*' was killed (SIGKILL). Exiting..."; exit 137 ;;
      15) echo "Command '$*' was terminated (SIGTERM). Exiting..."; exit 143 ;;
    esac
  elif [[ $status -ne 0 ]]; then
    echo "Command '$*' failed with exit code $status, continuing..." >&2
  fi
}

THREADS=""
#THREADS="--threads=192"

for i in $(seq -w 1 22); do
  NAME="jemalloc-$i" HACK_ALLOC=0 run_cmd build/reldebug/benchmark/benchmark_runner benchmark/tpch/sf1/q${i}.benchmark --sf $SF $THREADS
done
for i in $(seq -w 1 22); do
  NAME="hackalloc-$i" run_cmd build/reldebug/benchmark/benchmark_runner benchmark/tpch/sf1/q${i}.benchmark --sf $SF $THREADS
done
