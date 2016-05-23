.PHONY:clean
CC=g++
CFLAGS=-Wall -g -std=c++11
BIN=server
OBJS=buffer_server.o send.o data.o
LIBS=-levent
$(BIN):$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -rf *.o $(BIN)
