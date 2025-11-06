# use local nats (found in local_nats build by build-nats-c.sh)
USE_LOCAL_NATS?=1

TARGET = nats-receiver
CC = gcc
CFLAGS = -g -Wall -O3

INCLUDES += $(if $(USE_LOCAL_NATS),$(shell PKG_CONFIG_PATH="../local_nats/lib/pkgconfig" pkg-config --cflags libnats),$(shell pkg_config --cflags libnats))
LIBS = $(if $(USE_LOCAL_NATS),$(shell PKG_CONFIG_PATH="../local_nats/lib/pkgconfig" pkg-config --libs libnats),$(shell pkg_config --libs libnats))

LIBS += -lstdc++

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

# tiny-process-library
OBJECTS += tiny-process/process.o tiny-process/process_unix.o
CFLAGS += -Itiny-process

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

install: $(TARGET)
	mkdir -p $(DESTDIR)/usr/local/bin
	cp $(TARGET) $(DESTDIR)/usr/local/bin

clean:
	-rm -f *.o
	-rm -f tiny-process/*.o
	-rm -f $(TARGET)