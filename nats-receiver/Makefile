# use local nats (found in local_nats build by build-nats-c.sh)
USE_LOCAL_NATS?=1

TARGET  = nats-receiver
TARGET2 = nats-object
CC = gcc
CFLAGS = -g -Wall -O3

INCLUDES += $(if $(USE_LOCAL_NATS),$(shell PKG_CONFIG_PATH="../local_nats/lib/pkgconfig" pkg-config --cflags libnats),$(shell pkg_config --cflags libnats))
LIBS = $(if $(USE_LOCAL_NATS),$(shell PKG_CONFIG_PATH="../local_nats/lib/pkgconfig" pkg-config --libs libnats),$(shell pkg_config --libs libnats))
# LIBS = -L ../thirdparty/nats.c-3.11.0/build/lib -l natsd

.PHONY: default all clean

default: $(TARGET) $(TARGET2)
all: default

OBJECTS = nats-receiver.o
OBJECTS2 = nats-object.o

HEADERS = $(wildcard *.h)

# tiny-process-library
OBJECTS += tiny-process/process.o tiny-process/process_unix.o
OBJECTS2 += tiny-process/process.o tiny-process/process_unix.o
CFLAGS += -Itiny-process

%.o: %.cpp $(HEADERS)
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PRECIOUS: $(TARGET) $(TARGET2) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -Wall $(LIBS) -lstdc++ -lc -pthread -o $@

$(TARGET2): $(OBJECTS2)
	$(CXX) $(OBJECTS2) -Wall $(LIBS) -lstdc++ -lc -pthread -o $@

install: $(TARGET) $(TARGET2)
	mkdir -p $(DESTDIR)/usr/local/bin
	cp $(TARGET) $(DESTDIR)/usr/local/bin
	cp $(TARGET2) $(DESTDIR)/usr/local/bin


clean:
	-rm -f *.o
	-rm -f tiny-process/*.o
	-rm -f $(TARGET) $(TARGET2)