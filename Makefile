# CC will be set by env.mk if not set 
CC=
AR=ar
DEBUG=1
include env.mk
CFLAGS += 
LDFLAGS += 
SOURCES := $(wildcard src/*.c src/*/*.c)
LIB_SOURCES := $(wildcard src/*.c)
HEADERS := $(wildcard src/*.h src/*/*.h)
OBJECTS := $(subst .c,.o, $(subst src,build, $(SOURCES)))
LIB_OBJECTS := $(subst .c,.o, $(subst src,build, $(LIB_SOURCES)))

.Phony: clean

default: modi.a 

all: modi.a modi-dump modi-info modi-play

modi.a: $(LIB_OBJECTS)
	$(AR) rcs $@ $^

modi-dump: modi.a build/bin/dump.o
	$(CC) $^ -o $@ $(CFLAGS) -I ./include 

modi-info: modi.a build/bin/info.o
	$(CC) $^ -o $@ $(CFLAGS) -I ./include 


$(OBJECTS): ./build/%.o: ./src/%.c
	mkdir -p $(@D)
	$(CC) -c $< -o $@ $(CFLAGS) -I ./include 

clean:
	rm -rf build
	rm -f modi.a
	rm -f modi-dump