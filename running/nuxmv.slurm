#!/bin/bash
#SBATCH --job-name=nuXmv
#SBATCH --partition=interactive
#SBATCH --ntasks=1
#SBATCH --mem-per-cpu=8000m
#SBATCH --time=1:10:00
#SBATCH --export=ALL

cd $SLURM_SUBMIT_DIR
 
./nuxmv.pl $SLURM_ARRAY_TASK_ID
