# Compiler and flags
CXX = g++ 
CXXFLAGS = -Wall -g -I./include 

# Source and object directories
SRCDIR = src
OBJDIR = obj

# Get list of source files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES))

# Executable name
TARGET = json_parse

# Default rule to build the executable
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean up
clean:
	rm -rf $(OBJECTS)
	rm -f $(TARGET) 