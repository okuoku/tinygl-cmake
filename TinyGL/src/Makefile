OBJS= api.o list.o vertex.o init.o matrix.o texture.o \
      misc.o clear.o light.o clip.o select.o get.o error.o \
      zbuffer.o zline.o zdither.o ztriangle.o \
      zmath.o glx.o image_util.o
INCLUDES = -I../include
LIB = libTinyGL.a

all: $(LIB)

$(LIB): $(OBJS)
	rm -f $(LIB)
	ar rcs $(LIB) $(OBJS)
	cp $(LIB) ../lib

clean:
	rm -f *~ *.o *.a

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $*.c

clip.o: zgl.h
vertex.o: zgl.h
light.o: zgl.h
matrix.o: zgl.h
glx.o: zgl.h zglx.h
list.o: zgl.h opinfo.h
