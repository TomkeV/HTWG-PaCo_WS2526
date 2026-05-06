#
# 'make'        build executable file 'main'
# 'make clean'  removes all .o and executable files
#

# define the number of parallel jobs to use
MAKEFLAGS := --jobs=$(shell nproc)

# define the Cpp compiler to use
CXX = g++

# define any compile-time flags
CXXFLAGS	:= -std=c++17 -Wall -Wextra -g -fopenmp

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS = -lSDL2 -lSDL2main -lOpenCL 

# define output directory
OUTPUT	:= output

# directory for compiled object files
OBJDIR	:= bin

# define source directory
SRC		:= src

# define include directory
INCLUDE	:= include

# define lib directory
LIB		:= lib

ifeq ($(OS),Windows_NT)
MAIN	:= main.exe
SOURCEDIRS	:= $(SRC)
INCLUDEDIRS	:= $(INCLUDE)
LIBDIRS		:= $(LIB)
FIXPATH = $(subst /,\,$1)
RM			:= del /q /f
MD	:= mkdir
else
MAIN	:= main
SOURCEDIRS	:= $(shell find -L $(SRC) -type d 2>/dev/null)
INCLUDEDIRS	:= $(shell find -L $(INCLUDE) -type d 2>/dev/null)
LIBDIRS		:= $(shell find -L $(LIB) -type d 2>/dev/null)
FIXPATH = $1
RM = rm -f
MD	:= mkdir -p
endif

# define any directories containing header files other than /usr/include
INCLUDES	:= $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))

# define the C libs
LIBS		:= $(patsubst %,-L%, $(LIBDIRS:%/=%))

# define the C source files
SOURCES		:= $(wildcard $(patsubst %,%/*.cpp, $(SOURCEDIRS)))

# define the C object files (placed under `$(OBJDIR)` mirroring source paths)
OBJECTS		:= $(patsubst $(SRC)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

# define the dependency output files
DEPS		:= $(OBJECTS:.o=.d)
KERNELFILE	:= kernel.cl

#
# The following part of the makefile is generic; it can be used to
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

OUTPUTMAIN	:= $(call FIXPATH,$(OUTPUT)/$(MAIN))

all: $(OUTPUT) $(MAIN) $(KERNELFILE)
	@echo Executing 'all' complete!

$(OUTPUT):
	$(MD) $(OUTPUT)

$(MAIN): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(OUTPUTMAIN) $(OBJECTS) $(LFLAGS) $(LIBS)

# Copy Kernel from ../PythagorasTree/kernel.cl to Project directory
$(KERNELFILE): ../PythagorasTree/$(KERNELFILE)
	cp $< $(KERNELFILE)

# include all .d files
-include $(DEPS)

# this is a suffix replacement rule for building .o's and .d's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file)
# -MMD generates dependency output files same name as the .o file
# (see the gnu make manual section about automatic variables)
.PHONY: _dummy_for_obj_rule
# compile sources into OBJDIR preserving directory structure
$(OBJDIR)/%.o: $(SRC)/%.cpp
	$(MD) $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -MMD -MF $(patsubst %.o,%.d,$@) $< -o $@

.PHONY: clean
clean:
	$(RM) $(OUTPUTMAIN)
	$(RM) $(call FIXPATH,$(OBJECTS))
	$(RM) $(call FIXPATH,$(DEPS))
	@echo Cleanup complete!

run: all
	./$(OUTPUTMAIN)
	@echo Executing 'run: all' complete!
