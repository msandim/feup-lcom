# Makefile for the lab3 driver.

COMPILER_TYPE= gnu

CC=gcc

PROG=	lab3
SRCS=	lab3.c timer.c 

CCFLAGS= -Wall

DPADD+=	${LIBDRIVER} ${LIBSYS}
LDADD+=	-ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
