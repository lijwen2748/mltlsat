#!/bin/bash
#SBATCH --job-name=z3
#SBATCH --partition=interactive
#SBATCH --ntasks=1
#SBATCH --mem-per-cpu=8000m
#SBATCH --time=1:10:00
#SBATCH --export=ALL

cd $SLURM_SUBMIT_DIR
 
./z3.pl $SLURM_ARRAY_TASK_ID
