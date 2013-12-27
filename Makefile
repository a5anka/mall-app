CC = gcc
CFLAGS  = -g -Wall

INCLUDES =
LFLAGS =

LIBS = -lccn -lcrypto

# define the C source files
SRCS = server.c mallnode.c

OBJS = $(SRCS:.c=.o)

# define the executable file
MAIN = mallnode

.PHONY: depend clean

all:    $(MAIN)
	@echo  mallnode has been compiled

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
