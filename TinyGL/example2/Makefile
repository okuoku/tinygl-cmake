PROG = mech
OBJS = glu.o mech.o 

all: $(PROG)

clean:
	rm -f *.o *~ $(PROG)

$(PROG): $(OBJS) 
	$(CC) $(LFLAGS) $(OBJS) -o $(PROG) $(GL_LIBS) $(X_LIBS) -lm

.c.o:
	$(CC)	$(CFLAGS) $(GL_INCLUDES) $(X_INCLUDES) -c $*.c

mech.o: glu.h 

glu.o: glu.h
