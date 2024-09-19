# Define the compiler and flags
CXX = g++
# -Wall: Enable all warnings -std=c++17: Use the C++17 standard  
CXXFLAGS = -Wall -pedantic -std=c++17 -g -O2 #-fsanitize=address -fsanitize=undefined -D_GLIBCXX_DEBUG

# SFML libraries
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system 

# Define the target output and the source file
TARGET = sw.o
SRC = ./src/SimulationWrapper.cpp ./src/classes/CollisionShapes.cpp ./src/classes/SimulationWindow.cpp ./src/classes/ConfigurationWindow.cpp

# Default target to build the program
all: $(TARGET)

# Compile the program
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(SFML_LIBS)

# Clean the build files
clean:
	rm -f $(TARGET)
