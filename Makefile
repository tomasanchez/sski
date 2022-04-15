# C Makefile using gcc, gdb and valgrind.
# Modified version of Makefile using g++ & gdb by Roberto Nicolas Savinelli <rsavinelli@est.frba.utn.edu.ar>
# Tomas Agustin Sanchez <tosanchez@est.frba.utn.edu.ar>

# ! Avoid modifying this section - (Unless you know what you are doing) --------------------------------------------------------------

# Build directory - executables files will be stored in this directory
BUILD_DIR=build
# Log directory - log files will be stored in this directory
LOG_DIR=log
# Shared Library directory - shared source files are located in this directory
LIB_DIR=lib
# Shared object directory - shared compiled file objects will be stored in this directory
OBJ_DIR=shared
# Commons name - Operating System Course Library name.
COMMONS=so-commons-library
# Compile script - Custom make directive
MAKE_COMPILE = $(MAKE) compile --no-print-directory
# Test script - Custom make test directive
MAKE_TEST = $(MAKE) test --no-print-directory
# Test Scrpt for each application - Loop for all modules applying test directive.
# ? [modules].forEach( module => makeTest(module))
TEST_ALL=$(foreach dir, $(DIRS), cd $(dir) && $(MAKE_TEST) && cd .. &&)

# * Add your modules directories in this section -------------------------------------------------------------------------------------

# Modules directories - with their own makefile

# TODO: Add additional module directories below here
# ? eg. MEMORY_DIR=memory
KERNEL_DIR=kernel
CONSOLE_DIR=console
MEMORY_DIR=memory
CPU_DIR=cpu
# * DO NOT FORGET TO ADD YOUR DIRECTORIES HERE ---------------------------------------------------------------------------------------

#  Directories list
# ! Allways add your listed above directories here
# ? eg. DIRS =$(SERVER_DIR) $(CLIENT_DIR) $(MEMORY_DIR)
# TODO: Add the listed directories
DIRS = $(KERNEL_DIR) $(CPU_DIR)


# * DO NOT FORGET TO ADD YOUR RULES IN ALL --------------------------------------------------------------------------------------------

# All rules
# ! Allways add your rule for modules in here
# ? eg. all: server client memory filesystem etc
# TODO: add your rules
all: lib kernel cpu

# This targets are not files
# ! Allways add your rules for modules in here too
# ? eg. .PHONY: server client memory filesystem etc  [...] clean install test
# TODO: add your rules here
.PHONY: kernel clean install test lib cpu


# ! AVOID MODIFYING THIS SECTION ------------------------------------------------------------------------------------------------------

# This rule will be executed to build the different modules
compile:
		@mkdir -p $(BUILD_DIR)
		@mkdir -p $(LOG_DIR)
		@touch $(LOG_DIR)/$(KERNEL_DIR).log
		@touch $(LOG_DIR)/$(CONSOLE_DIR).log
		@touch $(LOG_DIR)/$(CPU_DIR).log
		@touch $(LOG_DIR)/$(MEMORY_DIR).log
		$(MAKE) all --no-print-directory

# This rule
test:
	$(TEST_ALL) true

# This rule will be executed remove the generated executables and objects files
clean:
	rm -fr $(BUILD_DIR)
	rm -fr $(OBJ_DIR)

# ? WATCH OUT MODIFYING THIS INSTALL SECTION --------------------------------------------------------------------------------------------------

# ! Requieres root user
# Customize the needed dependencies here.
install:
	@echo Installing dependencies...
# TODO: Install required libraries here.
	@echo "\nInstalling readline"
	apt-get install libreadline-dev
	@echo "\nReadline installed!\n"
	@echo "\nInstalling commons libraries...\n"
	@echo $(PWD)
	rm -rf $(COMMONS)
	git clone "https://github.com/sisoputnfrba/$(COMMONS).git" $(COMMONS)
	cd $(COMMONS) && sudo make uninstall --no-print-directory && sudo make install --no-print-directory && cd ..
	rm -rf $(COMMONS)
	@echo "\nCommons installed\n"
	@echo Completed

# ! Requieres root user
#Compile the shared library
lib:
	@echo "Building shared libraries...\n"
	rm -fr $(OBJ_DIR)
	cd $(LIB_DIR) && $(MAKE) test --no-print-directory && cd ..
	@echo "Shared libraries built!\n"

# TODO: Add modules rules below -------------------------------------------------------------------------------------------------------------------


# ? memory:
# ? cd $(MEMORY_DIR) && $(MAKE_COMPILE)
kernel:
	cd $(KERNEL_DIR) && $(MAKE_COMPILE)
cpu:
	cd $(CPU_DIR) && $(MAKE_COMPILE)
