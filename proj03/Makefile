# name of the binary
PROGRAM    = Fire

# source files
SRCS       = firestarter.c X-graph.c display.c

# object files from source files
OBJS       = $(SRCS:.c=.o)

# which compiler to use
CC         = mpicc

# flags for compilation
CFLAGS     = -Wall -I/usr/X11R6/include

# flags for linking
LFLAGS     = -o $(PROGRAM) -L/usr/X11R6/lib -lX11 -lm

# valid file suffixes 
.SUFFIXES: .c .o .cpp

all: $(PROGRAM)

# command to build .o files from .c files
.c.o: 
	module load openmpi-2.0/gcc; \
	$(CC) -c $(CFLAGS) $<

# command to link object files into binary
$(PROGRAM): $(OBJS)
	module load openmpi-2.0/gcc; \
	$(CC) $(OBJS) $(LFLAGS)

# other dependencies (based on #includes)
X-graph.o: X-graph.h display.h
display.o: display.h
firestarter.o: X-graph.h

clean:
	rm -f $(PROGRAM) a.out *~ *# *.o *.out slurm*
