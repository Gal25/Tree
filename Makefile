CC = gcc
AR = ar
FLAGS = -Wall -g

OBJECTS_stree = stree2.o

all: stree

stree: $(OBJECTS_stree)
	$(CC) $(FLAGS) -o stree $(OBJECTS_stree) 

.PHONY: clean all

clean:
	rm -f *.o *.so stree 