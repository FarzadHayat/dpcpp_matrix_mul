# Compiler
CXX = icpx

# Compiler flags
CXXFLAGS = -fsycl

# Target executable
TARGET = matrix_mul

# Source file
SRC = matrix_mul.cpp

# Default rule
all: $(TARGET)

# Rule to compile the source file
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Rule to run the executable
run: $(TARGET)
	./$(TARGET)

# Clean up generated files
clean:
	rm -f $(TARGET)

