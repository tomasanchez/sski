# C Makefile using gcc, gdb and valgrind.
# Modified version of Makefile using g++ & gdb by Roberto Nicolas Savinelli <rsavinelli@est.frba.utn.edu.ar>
# Tomas Agustin Sanchez <tosanchez@est.frba.utn.edu.ar>

# ? Most defined will not required further modifycation -----------------------------------------------------------------------------------

# C Compiler
CC = gcc
# Compiler Flags
CFLAGS = -Wall -Wextra -g3 -c
# Test Compiler flags
TCFLAGS = -Wall -Wextra -Wshadow -Wno-unused-variable -Wno-unused-function -Wno-unused-result -Wno-unused-variable -Wno-pragmas -O3 -g3
# Used libraries
# * Add libraries when needed
LIBS = -lcommons -pthread
#Log Folder
LOG_FOLDER=../log
# Build Folder
BUILD_DIRECTORY=../shared
# Include directory
INCLUDE_DIRECTORY=./inc
# Source directory
SOURCE_DIRECTORY=./src
# Test Directory
TEST_DIRECTORY=./test
# Inlcude folder
# ? Loops [includeDirectory].forEach(includeDirectory => concat("-I ", "includeDirectory"))
INCLUDES = $(foreach dir, $(shell find $(INCLUDE_DIRECTORY) -type d -print), $(addprefix -I , $(dir)))
# Source files
# ? Obtains all source files except MAIN
SOURCES = $(filter-out $(MAIN_FILE), $(shell find $(SOURCE_DIRECTORY) -name '*.c'))
# Test cases files
TESTS = $(shell find $(TEST_DIRECTORY) -name '*.c')
# Application name
APPNAME = lib
# Output file name
OUTPUT = $(BUILD_DIRECTORY)/$(APPNAME).o
# Test Output file
TEST_OUTPUT = $(BUILD_DIRECTORY)/$(APPNAME)_test.out
# Leaks log file
LEAKS = log/leaks.log
# Thread chek log file
HELGRIND = log/threads.log


all : compile

.PHONY: all

# ! Avoid modifying this section - (Unless you know what you are doing) --------------------------------------------------------------

# Compilation rule
# ? Generates .out executable
compile:
	@mkdir -p $(BUILD_DIRECTORY)
	@echo Building objects: [$(APPNAME)]...
	$(CC) $(CFLAGS) $(SOURCES) $(INCLUDES) $(LIBS) && mv -f *.o $(BUILD_DIRECTORY)
	@echo Build completed.

# Test compilation
# ? Generates *_test.out executable
test-build:
	@mkdir -p $(BUILD_DIRECTORY)
	@echo Preparing tests...
	$(MAKE) compile
	$(CC) $(TCFLAGS) $(TESTS) $(BUILD_DIRECTORY)/*.o $(INCLUDES) $(LIBS) -o $(TEST_OUTPUT)

# Runs tests cases
test: test-build
	./$(TEST_OUTPUT)
	@echo Tests completed.

# ! Uses Valgrind MemCheck tool
leaks: compile
	@mkdir -p log
	valgrind --leak-check=yes --log-file="$(LEAKS)" --track-origins=yes ./$(OUTPUT)

# ! Uses Valgrind Helgrind tool
threads: compile
	@mkdir -p log
	valgrind --tool=helgrind --log-file="$(HELGRIND)" ./$(OUTPUT)

# Removes Output files
clean:
	$(RM) ./$(OUTPUT)

# Removes log files
cleanLogs:
	$(RM) -r log || true

# Removes both logs and executables
remove: clean cleanLogs
