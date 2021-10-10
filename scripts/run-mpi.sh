#!/bin/bash

module add mpi/openmpi4-x86_64

for n in 1000 1000000 100000000
do
  for p in {1..8}
  do
    mpiexec -np $p bin/compute-integral $n 1
  done
done
