#!/bin/bash

less Makefile
module load openmpi/1.5

# check the compiler that is used underneath is GNU
mpicc --version

# run the code without scalasca
make clean
make
make run

# scalsca with GNU
module load scalasca/1.3.2-gnu
PRE=skin make

# run the experiment
PRE="scan -s -t" make run2

# move to new folder to prevent overwriting
mv epik_matmul_4_trace epik_matmul_4_trace_gnu

# GUI analysis report examination
square epik_matmul_4_trace_gnu

# score output: cube3_score
square -s epik_matmul_4_trace_gnu
less ./epik_matmul_4_trace_gnu/epik.score

# redo with intel compiler
# load the modules
module unload scalasca/1.3.2-gnu
module load intel/2011
module load scalasca/1.3.2-intel
# set OpenMPI to use intel compilers
export OMPI_CC=icc
export OMPI_CXX=icpc
export OMPI_F77=ifort
export OMPI_FC=ifort

# check if intel is used
mpicc --version 

# scalasca with Intel
make clean
PRE=skin make
PRE="scan -s -t" make run2
mv epik_matmul_4_trace epik_matmul_4_trace_intel
square epik_matmul_4_trace_intel

# show the differences of the 2 runs
cube3_diff epik_matmul_4_trace_gnu/summary.cube.gz epik_matmul_4_trace_intel/summary.cube.gz
cube3 diff.cube.gz 


