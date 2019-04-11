#!/bin/bash
#SBATCH --partition physical
#SBATCH --constraint=physg4
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=8
#SBATCH --time=0-00:10:00

module load OpenMPI/3.1.0-GCC-6.2.0

echo "TweetCount test on Physical partition"
echo
echo "nodes=1 ntasks-per-node=8"
echo
cd /home/zlp/CCC_a1
#mpic++ -O3 main.cpp
mpiexec  a.out
 
