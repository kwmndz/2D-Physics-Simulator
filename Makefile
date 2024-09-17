# Define the compiler and flags
CXX = g++
# -Wall: Enable all warnings -std=c++11: Use the C++11 standard  
CXXFLAGS = -Wall -std=c++11 -g 

# SFML libraries
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system 

# Define the target output and the source file
TARGET = sw.o
SRC = ./src/SimulationWrapper.cpp ./src/classes/CollisionShapes.cpp

# Default target to build the program
all: $(TARGET)

# Compile the program
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(SFML_LIBS)

# Clean the build files
clean:
	rm -f $(TARGET)
