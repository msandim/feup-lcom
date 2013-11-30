# Makefile for the lab4 driver.

COMPILER_TYPE= gnu

CC=gcc

PROG=	lab5
SRCS=	test5.c lab5.c timer.c mouse.c

CCFLAGS= -Wall

DPADD+=	${LIBDRIVER} ${LIBSYS}
LDADD+=	-ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
