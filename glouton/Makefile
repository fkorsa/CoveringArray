CC = g++
CFLAGS = -c -std=c++11
INCPATH = -I.
LIBS = 
OBJ =  cmatrix.o ca_solution.o main.o
TARGET = covering_array

all: $(TARGET)

clean:
	-rm -f *.o *~ core *.a

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LIBS) -o $(TARGET)

.SUFFIXES: .cpp.o

.cpp.o:
	$(CC) $(INCPATH) $(CFLAGS) -o $@ $<
