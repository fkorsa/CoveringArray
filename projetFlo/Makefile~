CC = g++
CFLAGS = -c -std=c++11
INCPATH = -I.
LIBS = 
OBJ =  CA_Solution.o main.o generateur.o generateur_descente.o generateur_glouton.o generateur_evolution.o generateur_recuit.o generateur_tabou.o resultats.o
TARGET = covering_array

all: $(TARGET)

clean:
	-rm -f *.o *~ covering_array *.a

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LIBS) -o $(TARGET)

.SUFFIXES: .cpp.o

.cpp.o:
	$(CC) $(INCPATH) $(CFLAGS) -o $@ $<
