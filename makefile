# Name of the project
PROJ_NAME=mini-shell
 
# .c files
C_SOURCE=$(wildcard ./src/*.c)
 
# .h files
H_SOURCE=$(wildcard ./src/*.h)
 
# Object files
OBJ=$(subst .c,.o,$(subst src,build,$(C_SOURCE)))
 
# Compiler and linker
CC=gcc
 
# Flags for compiler
CC_FLAGS=-c         \
         -W         \
         -Wall      \
         -ansi      \
         -pedantic  \
         -lreadline
 
# Command used at clean target
RM = rm -rf
 
#
# Compilation and linking
#
all: objFolder $(PROJ_NAME)
 
$(PROJ_NAME): $(OBJ)
    @ echo 'Building binary using GCC linker: $@'
    $(CC) $^ -o $@
    @ echo 'Finished building binary: $@'
    @ echo ' '
 
./build/%.o: ./src/%.c ./src/%.h
    @ echo 'Building target using GCC compiler: $<'
    $(CC) $< $(CC_FLAGS) -o $@
    @ echo ' '
 
./build/main.o: ./src/main.c $(H_SOURCE)
    @ echo 'Building target using GCC compiler: $<'
    $(CC) $< $(CC_FLAGS) -o $@
    @ echo ' '
 
objFolder:
    @ mkdir -p build
 
clean:
    @ $(RM) ./build/*.o $(PROJ_NAME) *~
    @ rmdir build
 
.PHONY: all clean
