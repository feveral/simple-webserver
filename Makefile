CC = gcc
CFLAGS = -O2 -g -Wall -I include
OBJS = \
		src/main.o \
		src/list.o \
		src/server.o \
		src/utility.o \
		src/request.o \
		src/response.o \
		src/kv.o \
		src/handler.o \

EXEC = webserver

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

all: $(EXEC)
run: $(EXEC)
	sudo ./$(EXEC) 80

clean:
	rm $(EXEC) $(OBJS)