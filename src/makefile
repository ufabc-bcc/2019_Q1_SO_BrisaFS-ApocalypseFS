# Sets current compiler
CC = gcc

## Compiler flags
# -g      : Adds debugging information to the executable file
# -Wall   : Displays most of the compiler warnings (but not everyone)
# -Werror : Every warning is treated as an error
# -O3     : Optimizes code size and execution time [most for execution time] (for code size, change either to -O1)
# CFLAGS = -Wall -Werror -O3 -g -I $(HEADERS)
CFLAGS = -Wall -Werror -O3 -g

# Build target executable
TARGET = apocalypsefs.run

# Dependencies
MAIN = apocalypse_main.c
FUNCTIONS = apocalypse_functions
FUNCTIONS_P = ./$(FUNCTIONS)/$(FUNCTIONS)
DIR_FUNCTIONS = dir_functions
DIR_FUNCTIONS_P = ./$(DIR_FUNCTIONS)/$(DIR_FUNCTIONS)
UTILITIES_FUNCTIONS = global_utilities
UTILITIES_FUNCTIONS_P = ./$(UTILITIES_FUNCTIONS)/$(UTILITIES_FUNCTIONS)

# Configs
CONFIGS = `pkg-config fuse3 --cflags --libs`

# Mounting configs
MOUNTC = -f -d -s

# Output dir
OUT = ./ap_out

# Configures RUN
ifndef MOUNT
MOUNT = /tmp/apocalypse_mount
$(shell mkdir -m 777 -p $(MOUNT))
# $(shell [ ! -d $(MOUNT) ] && mkdir -m 777 -p $(MOUNT))
endif

default: $(TARGET)

$(TARGET): $(MAIN) $(FUNCTIONS_P).c
	@$(CC) $(CFLAGS) $(DIR_FUNCTIONS_P).c $(UTILITIES_FUNCTIONS_P).c $(FUNCTIONS_P).c $(MAIN) -o $(TARGET) $(CONFIGS)
	@echo "Completed!"

#$(FUNCTIONS).o: $(FUNCTIONS_P).c
#	$(CC) $(CFLAGS) $(FUNCTIONS_P).c -o $(FUNCTIONS).o $(CONFIGS)

run:
	./$(TARGET) $(MOUNTC) $(MOUNT)

clean:
	@$(RM) *.run *.o *.out
	$(info Done!)
