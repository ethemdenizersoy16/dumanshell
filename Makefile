CC = gcc
CFLAGS = -Wall



TARGET = dumanshell


OBJS = dumanshell.o history.o read_input.o pipe.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)


dumanshell.o : dumanshell.c read_input.h history.h
history.o : history.c history.h
read_input.o : read_input.c read_input.h
pipe.o : pipe.c pipe.h

clean:
	rm -f $(TARGET) $(OBJS) 
