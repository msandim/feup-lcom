# Makefile for the lab4 driver.

COMPILER_TYPE= gnu

CC=gcc

PROG=	lab6
SRCS=	lab6.c test6.c rtc.c rtc_assembly.S

CCFLAGS= -Wall

DPADD+=	${LIBDRIVER} ${LIBSYS}
LDADD+=	-ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
