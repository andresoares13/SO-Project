CC = gcc
CFLAGS = -Wall
OBJS = phrases.o
EXEC = phrases

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@


.PHONY : clean
clean :
	-rm $(OBJS) $(EXEC)
