# Makefile for the project solution.

COMPILER_TYPE= gnu

CC=gcc

PROG=	proj

#DEVICES
SRCS=	main.c video_gr.c vbe.c mouse.c keyboard.c timer.c rtc.c rtc_assembly.S serial.c

# MODULES
SRCS+= central_module.c graphic_module.c draw_module.c user_interaction.c time_module.c com_module.c gallery_module.c

# EXTRA STRUCTURES
SRCS+= stack.c

CCFLAGS= -Wall

DPADD+=	${LIBDRIVER} ${LIBSYS}
LDADD+=	-llm -ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
