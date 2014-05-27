MF=	Makefile

CC=	$(PRE) mpicc

CFLAGS=	-O0 -DDEBUG=3 -g -Wall

LFLAGS=	$(CFLAGS)

EXE=	matmul

SRC= \
	helper.c	\
    	mpicomm.c   \
    	matrix.c    \
	matmul.c

FLS = \
    	helper.h    \
    	matrix.h    \
    	mpicomm.h

#
# No need to edit below this line
#

.SUFFIXES:
.SUFFIXES: .c .o

OBJ=	$(SRC:.c=.o)

.c.o:
	$(CC) $(CFLAGS) -c $<

all:	$(EXE)

$(EXE):	$(OBJ)
	$(CC) $(LFLAGS) -o $@ $(OBJ)

$(OBJ):	$(MF)

tar:
	tar cvjf $(EXE).tar.bz2 $(MF) $(SRC) $(FLS)

run:
#	-mpirun -x OUTPUT=0 -x PROBLEMLIST=4-300/1 -np 4 ./matmul | tee output.dat ; gnuplot gnuplot.template
	-mpirun -x OUTPUT=0 -x PROBLEMLIST=4-300/1 -np 4 ./matmul


run2:
#	-$(PRE) mpirun "-x OUTPUT=0" "-x PROBLEMLIST=\"4-300/1\"" -np 4 ./matmul | tee output.dat ; gnuplot gnuplot.template
	-$(PRE) mpirun "-x OUTPUT=0" "-x PROBLEMLIST=\"4-300/1\"" -np 4 ./matmul

clean:
	rm -f $(OBJ) $(EXE) core *.z *.otf
