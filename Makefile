CC = gcc
CXX = g++
RM = rm -f
MKDIR = mkdir -p

DEBUG_FLAGS = -gdwarf-2 -O0 -Wall
RELEASE_FLAGS = -O2 -Wall -DNDEBUG

INCLUDES = -I. -I./test/include -I./UOCPlay/include
LIBS = -L./lib -lUOCPlay

SRC_FILES = src/main.c \
            test/src/test_pr3.c \
            test/src/test_pr2.c \
            test/src/test_suite.c \
            test/src/test_pr1.c \
            test/src/test.c

DEBUG_TARGET = ./bin/UOC20242d
RELEASE_TARGET = ./bin/UOC20242

.PHONY: all clean debug release

all: debug

debug: | bin
	$(CC) $(DEBUG_FLAGS) $(INCLUDES) $(SRC_FILES) $(LIBS) -o $(DEBUG_TARGET)

release: | bin
	$(CC) $(RELEASE_FLAGS) $(INCLUDES) $(SRC_FILES) $(LIBS) -o $(RELEASE_TARGET)

bin:
	$(MKDIR) bin

clean:
	$(RM) $(DEBUG_TARGET) $(RELEASE_TARGET)cat > Makefile << 'EOF'
