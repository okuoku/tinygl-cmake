#####################################################################
# C compiler

CC= gcc
CFLAGS= -g -Wall -O2
LFLAGS=

#####################################################################
# TinyGL configuration 

# big endian computer (i.e. sparc, mips, ppc) . The default is little
# endian (x86, alpha)
#OPT_CFLAGS= -DBIG_ENDIAN

#####################################################################
# X11 configuration (for the examples only)

# Linux
X_LIBS= -L/usr/X11/lib -lX11 -lXext
X_INCLUDES= 
# Solaris
#X_LIBS= -L/usr/X11/lib -lX11 -lXext -lsocket -lnsl
#X_INCLUDES= 

#####################################################################
# OpenGL configuration (for the examples only)

# use TinyGL 
GL_LIBS= -L../lib -lTinyGL 
GL_INCLUDES= -I../include

# use Mesa
#GL_LIBS= -lMesaGL 
#GL_INCLUDES= 

# use OpenGL
#GL_LIBS= -lGL 
#GL_INCLUDES= 

#####################################################################
# Do not modify after this

DIRS= src example1 example2

all:
	( for f in $(DIRS); do ( cd $$f ; \
  make GL_LIBS='$(GL_LIBS)' GL_INCLUDES='$(GL_INCLUDES)' \
       X_LIBS='$(X_LIBS)' X_INCLUDES='$(X_INCLUDES)' \
	     CC='$(CC)' CFLAGS='$(CFLAGS) $(OPT_CFLAGS)' LFLAGS='$(LFLAGS)' \
       all \
  ) done )

clean:
	rm -f *~ lib/libTinyGL.a include/GL/*~
	( for f in $(DIRS); do ( cd $$f ; make clean ; ) done )

install:
	( for f in $(DIRS); do ( cd $$f ; make install ; ) done )
