CXX=mpic++
MPIRUN=mpirun
CXXFLAGS=-std=c++14
LIBS=-lboost_mpi -lboost_serialization


TARGET=Numbskull
SRC=$(wildcard ../*.cpp)
OBJ=$(SRC:%.cpp=%.o)


all: $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LIBS)

clean:
	rm -f ../*.o
	rm -f $(TARGET)

run: all
	$(MPIRUN) -np 2 $(TARGET)

