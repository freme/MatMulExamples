MF=	Makefile

CC=	$(PRE) cc

CFLAGS=	-Os -DDEBUG=0 -Wall
#CFLAGS=	-O0 -g -DDEBUG=3 -Wall

LFLAGS=	$(CFLAGS)

EXE=	matmul

SRC= \
		helper.c	\
    	matrix.c    \
		matmul.c

FLS = \
    	helper.h    \
    	matrix.h    \

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
	-export OUTPUT=0; export PROBLEMLIST=4-300/1; ./matmul


run2:
	-export OUTPUT=0; export PROBLEMLIST=4-300/1; $(PRE) ./matmul

clean:
	rm -f $(OBJ) $(EXE) core *.z *.otf
