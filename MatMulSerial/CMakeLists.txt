cmake_minimum_required(VERSION 2.6)


SET(SRC 
  helper.c 
  matrix.c 
  matmul.c
)

SET(FLS helper.h matrix.h matmul.h)

SET(EXENAME matmul)

PROJECT(MatMulMPISerial)
ADD_EXECUTABLE(${EXENAME} ${SRC})
ADD_DEFINITIONS("-g -DDEBUG=0 -Wall")
#ADD_DEFINITIONS("-g -DDEBUG=0 -Wall -C")
#SET(CMAKE_C_COMPILER mpicc)
#SET(CMAKE_CXX_COMPILER mpicxx)

#SET(CMAKE_C_COMPILER vtcc)
#SET(CMAKE_C_FLAGS "-vt:cc mpicc")
##SET(CMAKE_VERBOSE_MAKEFILE true)

#target_link_libraries(${EXENAME} mpi) 

