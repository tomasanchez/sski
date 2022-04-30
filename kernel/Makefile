# C Makefile using gcc, gdb and valgrind.
# Modified version of Makefile using g++ & gdb by Roberto Nicolas Savinelli <rsavinelli@est.frba.utn.edu.ar>
# Tomas Agustin Sanchez <tosanchez@est.frba.utn.edu.ar>

# ? Most defined will not required further modifycation -----------------------------------------------------------------------------------

# C Compiler
CC = gcc
# Compiler Flags
CFLAGS = -Wall -Wextra -g3
# Test Compiler flags
TCFLAGS = -Wall -Wextra -Wshadow -Wno-unused-variable -Wno-unused-function -Wno-unused-result -Wno-unused-variable -Wno-pragmas -O3 -g3
# Used libraries
# * Add libraries when needed
LIBS = -lcommons -pthread
#Log Folder
LOG_FOLDER=../log
# Build Folder
BUILD_FOLDER=../build
# Include directory
INCLUDE_DIRECTORY=./inc/ ../lib/inc/
# Source directory
SOURCE_DIRECTORY=./src
# Test Directory
TEST_DIRECTORY=./test
# The main file path
MAIN_FILE= ./src/main.c
# The shared library source directory
LIB_DIRECTORY= ../lib/src
# Inlcude folder
# ? Loops [includeDirectory].forEach(includeDirectory => concat("-I ", "includeDirectory"))
INCLUDES = $(foreach dir, $(shell find $(INCLUDE_DIRECTORY) -type d -print), $(addprefix -I , $(dir)))
# The shared library source files
LIB_SOURCES = ../shared/*.o
# Source files
# ? Obtains all source files except MAIN
SOURCES = $(filter-out $(MAIN_FILE), $(shell find $(SOURCE_DIRECTORY) -name '*.c')) $(LIB_SOURCES)
# Test cases files
# ? Obtains all test source files
TESTS = $(shell find $(TEST_DIRECTORY) -name '*.c')
# Module name
# ! auto-generated module name (name of directory)
# ? eg. /user/home/dev/so-starter/memory will be memory
MODULE_NAME = $(notdir $(shell pwd))
# Output file name
# ? eg. memory.out
OUTPUT = $(BUILD_FOLDER)/$(MODULE_NAME).out
# Test Output file
# ? eg. memory_test.out
TEST_OUTPUT = $(BUILD_FOLDER)/$(MODULE_NAME)_test.out
# Leaks log file
LEAKS = $(LOG_FOLDER)/$(MODULE_NAME)_leaks.log
# Thread chek log file
HELGRIND = $(LOG_FOLDER)/$(MODULE_NAME)_threads.log
# Log File
OUTPUT_LOG = $(LOG_FOLDER)/$(MODULE_NAME).log

all : compile run

.PHONY: all

# ! Avoid modifying this section - (Unless you know what you are doing) --------------------------------------------------------------

# Compilation rule
# ? Generates .out executable
compile:
	@mkdir -p $(BUILD_FOLDER)
	@echo Building module: [$(MODULE_NAME)]...
	$(CC) $(CFLAGS) $(MAIN_FILE) $(SOURCES) $(INCLUDES) $(LIBS) -o $(OUTPUT)
	@echo Build completed.

# Compiles & Runs executable
run: compile
	./$(OUTPUT)

# Test compilation
# ? Generates *_test.out executable
test-build:
	@mkdir -p $(BUILD_FOLDER)
	@echo Preparing tests...
	$(CC) $(TCFLAGS) $(TESTS) $(SOURCES) $(INCLUDES) $(LIBS) -o $(TEST_OUTPUT)
	@echo Test builds!

# Runs tests cases
test: test-build
	@echo Running test cases...
	./$(TEST_OUTPUT)
	@echo Tests completed.

# ! Uses Valgrind MemCheck tool
leaks: compile
	@mkdir -p $(LOG_FOLDER)
	valgrind --leak-check=yes --log-file="$(LOG_FOLDER)/$(LEAKS)" --track-origins=yes --leak-check=full --show-leak-kinds=all ./$(OUTPUT)

# ! Uses Valgrind Helgrind tool
threads: compile
	@mkdir -p $(LOG_FOLDER)
	valgrind --tool=helgrind --log-file="$(LOG_FOLDER)/$(HELGRIND)" ./$(OUTPUT)

# Removes Output files
clean:
	$(RM) ./$(OUTPUT)
	$(RM) ./$(TEST_OUTPUT)

# Removes log files
cleanLogs:
	$(RM) -r $(LOG_FOLDER) || true

# Removes both logs and executables
remove: clean cleanLogs
