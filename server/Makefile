CC = gcc
EXE = server
LDFLAGS =

# set DEBUG options
ifdef DEBUG
	CFLAGS = -Wall -Wextra -ggdb -pg -DDEBUG
else
	CFLAGS = -Wall -O2 
endif

# name all the object files
OBJS = main.o poll.o socket.o writen.o

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

debug:
	make all DEBUG=1

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

clean:
	rm -rf $(OBJS) $(EXE)
