PROG = texobj
OBJS = texobj.o

all: $(PROG)

clean:
	rm -f core *.o *~ $(PROG)

$(PROG): $(OBJS) 
	$(CC) $(LFLAGS) $(OBJS) -o $(PROG) $(GL_LIBS) $(X_LIBS) -lm

.c.o:
	$(CC)	$(CFLAGS) $(GL_INCLUDES) $(X_INCLUDES) -c $*.c
