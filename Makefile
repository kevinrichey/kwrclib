
# Compiler

CC = clang
CWARNFLAGS = -Wall -Wextra \
			 -Werror=return-type \
			 -Wno-missing-field-initializers \
			 -Wno-missing-braces
CFLAGS = -std=c11 -g -D DEBUG -I/mingw64/include/SDL2  -MMD $(CWARNFLAGS)
# -MMD = generate dependency rules

# Linker 

#LDFLAGS = -L/mingw64/lib -Wl,-subsystem,windows
LDLIBS = -lSDL2main -lSDL2

# Source Files

SOURCE = kwrmaze.c kwrlib.c
OBJ = $(SOURCE:.c=.o)
#LINK.o = $(LINK.cc)

# Targets

all: run-test run

run-test: test
	./test

run: main
	./main

test: test.o $(OBJ)

main: main.o $(OBJ)

remake: clean main

clean:
	rm -f *.exe *.o *.d

.PHONY: all clean run run-test

# Include the .d dependency files

include $(wildcard $(SOURCE:.c=.d))
