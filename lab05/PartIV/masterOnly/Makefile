PROG   = masterOnly
SRC    = $(PROG).c

CC     = gcc
CFLAGS = -Wall -ansi -pedantic -std=c99
LFLAGS = -o $(PROG) -fopenmp

$(PROG): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LFLAGS)

clean:
	rm -f $(PROG) *.o *~ *#


