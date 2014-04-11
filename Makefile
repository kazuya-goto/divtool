CC = gcc
CFLAGS = -Wall
LDFLAGS =

RM = rm -f

TARGET = divtool

SRCS = divtool.c
OBJS = $(SRCS:.c=.o)

.SUFFIXES: .o .c
.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

clean:
	$(RM) $(OBJS) $(TARGET)

test:
	sh test.sh
