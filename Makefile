# CC will be set by env.mk if not set 
CC=
AR=ar
DEBUG=1
FEATURE_FLAGS := -DMODI_FS_SUPPORT # possible: -DMODI_16BIT_SUPPORT -DMODI_FS_SUPPORT
include env.mk
CFLAGS += 
LDFLAGS += 
SOURCES := $(wildcard src/*.c src/*/*.c)
LIB_SOURCES := $(wildcard src/*.c)
HEADERS := $(wildcard src/*.h src/*/*.h)
OBJECTS := $(subst .c,.o, $(subst src,build, $(SOURCES)))
LIB_OBJECTS := $(subst .c,.o, $(subst src,build, $(LIB_SOURCES)))

.Phony: clean test

default: modi.a 

all: modi.a modi-dump modi-info modi-play

modi.a: $(LIB_OBJECTS)
	$(AR) rcs $@ $^

modi-dump: modi.a build/bin/dump.o
	$(CC) $^ -o $@ $(CFLAGS) $(FEATURE_FLAGS) -I ./include 

modi-info: modi.a build/bin/info.o
	$(CC) $^ -o $@ $(CFLAGS) $(FEATURE_FLAGS) -I ./include 

$(OBJECTS): ./build/%.o: ./src/%.c
	mkdir -p $(@D)
	$(CC) -c $< -o $@ $(CFLAGS) $(FEATURE_FLAGS) -I ./include 

# Tests
TEST_SOURCES :=  $(wildcard test/*.c) 
TEST_HEADERS := $(wildcard test/*.h)

test: build/tests
	./build/tests

build/tests: $(TEST_SOURCES) $(TEST_HEADERS) modi.a
	mkdir -p $(@D)
	$(CC) modi.a $(TEST_SOURCES) -o $@ -Wall $(FEATURE_FLAGS) -I ./include -I ./vendor/tau


clean:
	rm -rf build
	rm -f modi.a
	rm -f modi-dump