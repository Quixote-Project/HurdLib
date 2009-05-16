# ***************************************************************************
# (C)Copyright 2003, The Open Hurderos Foundation. All rights reserved.
# ***************************************************************************


# Variable declarations.
CSRC =	Buffer.c Fibsequence.c Origin.c String.c Config.c basic-parser.c

LIBNAME = HurdLib
LIBRARY = lib${LIBNAME}.a

CC = gcc

# Uncomment the following two lines to enable compilation with memory debug
# support
#
# DMALLOC = -DDMALLOC
# DMALLOC_LIBS = -ldmalloc

#
# Locations of SSL include files and libraries
#
SSL_INCLUDE = /usr/local/ssl/include
SSL_LIBRARY = -L /usr/local/ssl/lib -l ssl

CDEBUG = -O2 -fomit-frame-pointer -march=pentium2 ${DMALLOC}
CDEBUG = -g ${DMALLOC}

CFLAGS = -Wall ${CDEBUG} -I../include # -pedantic-errors -ansi

LDFLAGS = -g ${DMALLOC_LIBS}


#
# Compilation directives.
#
%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@;


#
# Automatic definition of classes and objects.
#
COBJS = ${CSRC:.c=.o}


# Targets
all: ${LIBRARY}

${LIBRARY}: ${COBJS}
	ar r ${LIBRARY} $^;
	ranlib ${LIBRARY};

basic-parser.c: basic-parser.l
	flex -o$@ $?;

tags:
	/opt/emacs/bin/etags *.{h,c};

clean:
	/bin/rm -f *.o *~ basic-parser.c TAGS ${LIBRARY}


# Source dependencies.
Buffer.o: ${LIBNAME}.h Origin.h Fibsequence.h
Fibsequence.o: ${LIBNAME}.h Origin.h Fibsequence.h
Origin.c: ${LIBNAME}.h Origin.h
String.c: ${LIBNAME}.h Origin.h Buffer.h String.h
Config.c: ${LIBNAME}.h Origin.h Config.h
