#!/bin/bash
#
#SBATCH --ntasks=8
#SBATCH --cpus-per-task=1
#SBATCH --partition=RT
#SBATCH --job-name=shishqatask1
#SBATCH --comment="Run mpi for task 1 | m.shishatskiy"
#SBATCH --output=output.txt
#SBATCH --error=error.txt

/bin/bash scripts/run-mpi.sh
