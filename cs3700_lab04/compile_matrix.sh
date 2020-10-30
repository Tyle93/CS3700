#!/bin/bash
#SBATCH --job-name=COMPILE_LOG
#SBATCH --output=COMPILE_LOG_%j.txt
#SBATCH --partition=gpu
#SBATCH --gres=gpu:gp100gl:2
#SBATCH --time=00:02:00

. /etc/profile.d/modules.sh

module load cuda/10.2

nvcc -o matrix matrix.cu

