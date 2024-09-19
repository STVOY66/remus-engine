INC := -I ./src -I ./deps/raylib #include directory flags
LIB := -L ./src -L ./deps/raylib #library directory flags
LFLAGS := -lraylib -lopengl32 -lgdi32 -lwinmm #linking flags
WFLAGS := -Wno-narrowing #Remove narrowing warnings
CFLAGS = $(INC) $(LIB) $(LFLAGS) #consolidates g++ flags

OBJDIR := src/objs
SRCDIR := src
SRC := $(wildcard $(SRCDIR)/*.cpp)
OBJ := $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

main: $(OBJ) #compiles executable using objects
	@echo "Linking executable..."
	@g++ -o main $^ $(CFLAGS)

$(OBJDIR)/%.o: src/%.cpp #object compilation
	@echo "Compiling object $@..."
	@g++ $(WFLAGS) -c $< $(CFLAGS) -o $@

.phony: clean
clean: #clean function to cleanup aftermath of makefile testing
	@echo "Deleting objects and executables"
	@rm -f $(OBJDIR)/*.o *.exe