# Makefile for the project solution.

COMPILER_TYPE= gnu

CC=gcc

PROG=	proj
SRCS=	video_gr.c vbe.c mouse.c graphic_module.c central_module.c user_interaction.c main.c

CCFLAGS= -Wall

DPADD+=	${LIBDRIVER} ${LIBSYS}
LDADD+=	-llm -ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
