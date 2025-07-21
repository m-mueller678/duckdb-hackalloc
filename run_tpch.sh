#!/bin/bash

SF=${SF:-1}

for i in $(seq -w 1 22); do
  build/release/benchmark/benchmark_runner benchmark/tpch/sf1/q${i}.benchmark --sf $SF
done
for i in $(seq -w 1 22); do
  HACK_ALLOC=1 build/release/benchmark/benchmark_runner benchmark/tpch/sf1/q${i}.benchmark --sf $SF
done