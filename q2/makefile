CC = gcc
CFLAGS = -Wall
OBJS = addmx.o
EXEC = addmx

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@


.PHONY : clean
clean :
	-rm $(OBJS) $(EXEC)
