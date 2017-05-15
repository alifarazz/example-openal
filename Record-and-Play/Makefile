
# change application name here (executable output name)
TARGET=captureAndPlayback

# compiler
CC=gcc
# debug
DEBUG=-ggdb
# optimisation
OPT=-O2
# warnings
WARN=-Wall
# standards
STD=-ansi -pedantic
#
PTHREAD=-pthread

CCFLAGS=$(DEBUG) $(WARN) $(PTHREAD) $(STD) -pipe `pkg-config --cflags freealut` `pkg-config --cflags openal` -Wall -Wextra


# linker
LD=gcc
LDFLAGS=$(PTHREAD) -export-dynamic `pkg-config --libs freealut` `pkg-config --libs openal`

OBJS=$(TARGET).o

all: $(OBJS)
	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS)

$(TARGET).o: $(TARGET).c
	$(CC) -c $(CCFLAGS) $(TARGET).c  -o $(TARGET).o

clean:
	rm -f *.o $(TARGET)
