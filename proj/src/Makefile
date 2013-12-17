# Makefile for the project solution.

COMPILER_TYPE= gnu

CC=gcc

PROG=	proj
SRCS=	main.c video_gr.c vbe.c mouse.c keyboard.c timer.c
SRCS+= central_module.c graphic_module.c draw_module.c user_interaction.c

CCFLAGS= -Wall

DPADD+=	${LIBDRIVER} ${LIBSYS}
LDADD+=	-llm -ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
