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
#ADD_DEFINITIONS("-g -DDEBUG=0 -Wall")
ADD_DEFINITIONS("-fast")
#ADD_DEFINITIONS("-xHOST -O3 -ipo -no-prec-div -static -fno-alias -unroll-aggressive -inline-level=2")
#ADD_DEFINITIONS("-g -DDEBUG=0 -Wall -C")
#SET(CMAKE_C_COMPILER mpicc)
#SET(CMAKE_CXX_COMPILER mpicxx)

SET(CMAKE_C_COMPILER vtcc)
#SET(CMAKE_C_FLAGS "-vt:cc gcc -DVTRACE")
#SET(CMAKE_C_COMPILER gcc)
SET(CMAKE_VERBOSE_MAKEFILE true)


IF(UNIX)
    TARGET_LINK_LIBRARIES(${EXENAME} m)
ENDIF(UNIX)

