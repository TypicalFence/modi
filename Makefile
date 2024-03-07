# CC will be set by env.mk if not set 
CC= 
DEBUG=1
include env.mk
CFLAGS += 
LDFLAGS += 
SOURCES := $(filter-out src/bin/dump.c, $(wildcard src/*.c src/*/*.c))
HEADERS := $(wildcard src/*.h src/*/*.h)
OBJECTS := $(subst .c,.o, $(subst src,build, $(SOURCES)))

.Phony: clean

default: modi.a 

modi.a: $(OBJECTS) 
	$(CC) -shared $^ $(LDFLAGS) -o modi.a

build/bin/dump.o: ./build/%.o: ./src/%.c
	mkdir -p $(@D)
	$(CC) -c $< -o $@ $(CFLAGS) -I ./include $(LIB_INCLUDE)

modi-dump: modi.a build/bin/dump.o
	$(CC) build/bin/dump.o modi.a -o $@ $(CFLAGS) -I ./include 

$(OBJECTS): ./build/%.o: ./src/%.c
	mkdir -p $(@D)
	$(CC) -c $< -o $@ $(CFLAGS) -I ./include $(LIB_INCLUDE)

clean:
	rm -rf build
	rm -f modi.a
	rm -f modi-dump