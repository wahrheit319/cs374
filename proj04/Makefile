# Names of the programs
PROG1 = squareAndSum
PROG2 = squareAndSumSeqBinary
PROG3 = squareAndSumParBinary

# Compilers
CC = mpicc    # For C programs
CXX = mpicxx  # For C++ programs

# Flags
CFLAGS = -Wall -ansi -pedantic -std=c99
CXXFLAGS = -Wall -pedantic

# Default target
all: $(PROG1) $(PROG2) $(PROG3)

# Target for squareAndSum (C code)
$(PROG1): $(PROG1).c
	$(CC) $(CFLAGS) $(PROG1).c -o $(PROG1)

# Target for squareAndSumSeqBinary (C code)
$(PROG2): $(PROG2).c
	$(CC) $(CFLAGS) $(PROG2).c -o $(PROG2)

# Target for squareAndSumParBinary (C++ code)
$(PROG3): $(PROG3).cpp
	$(CXX) $(CXXFLAGS) $(PROG3).cpp -o $(PROG3)

# Clean target
clean:
	rm -f $(PROG1) $(PROG2) $(PROG3) a.out *~ *.o *#

# Additional clean target for text files
cleanText:
	rm -f $(PROG1) $(PROG2) $(PROG3) *.txt
