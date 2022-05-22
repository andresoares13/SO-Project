CC = gcc
CFLAGS = -Wall
OBJS = cypher.o
EXEC = cypher

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@


.PHONY : clean
clean :
	-rm $(OBJS) $(EXEC)
