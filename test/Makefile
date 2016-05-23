.PHONY:clean
CC=gcc
CFLAGS=-Wall -g
BIN=client
OBJS=client.o
LIBS=
$(BIN):$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -rf *.o $(BIN)
