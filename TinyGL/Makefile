#####################################################################
# C compiler

CC= gcc
CFLAGS= -g -Wall -O2
LFLAGS=

# for BeOS PPC
#CC= mwcc
#CFLAGS= -I. -i-
#LFLAGS=

#####################################################################
# TinyGL configuration 

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

####################################################################
# Compile and link control

# UNIX systems
DIRS= src example1 example2

# BeOS
# DIRS= src BeOS

#####################################################################
# Do not modify after this

all:
	( for f in $(DIRS); do ( cd $$f ; \
  make GL_LIBS='$(GL_LIBS)' GL_INCLUDES='$(GL_INCLUDES)' \
       X_LIBS='$(X_LIBS)' X_INCLUDES='$(X_INCLUDES)' \
	     CC='$(CC)' CFLAGS='$(CFLAGS) $(OPT_CFLAGS)' LFLAGS='$(LFLAGS)' \
       all \
  ) || exit 1 ; done )

clean:
	rm -f *~ lib/libTinyGL.a include/GL/*~
	( for f in $(DIRS); do ( cd $$f ; make clean ; ) done )

install:
	( for f in $(DIRS); do ( cd $$f ; make install ; ) done )
