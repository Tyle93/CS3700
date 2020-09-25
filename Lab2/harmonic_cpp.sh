#!/bin/bash
#SBATCH --job-name=HARMONIC_CPP # Job name
#SBATCH --output=HARMONIC_CPP_%j.log # Log file name
#SBATCH --partition=compute # Use computing cluster
#SBATCH --mem=1gb # Job memory request
#SBATCH --nodes=5 # Number of computing nodes
#SBATCH --time=00:02:00 # Time limit HH:MM:SS

. /etc/profile.d/modules.sh

module load openmpi/2.1.2

/opt/openmpi-2.1.2/bin/mpirun ./harmonic.exe
