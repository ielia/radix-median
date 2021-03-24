CC=gcc
CFLAGS=-lm

HEADERS=logging.h median.h

PTARGET=median
TTARGET=test

all: $(PTARGET) $(TTARGET)

runtest: $(TTARGET)
	./$(TTARGET)

logging.o: logging.c $(HEADERS)
	$(CC) $(CFLAGS) -c logging.c

$(PTARGET).o: $(PTARGET).c $(HEADERS)
	$(CC) $(CFLAGS) -c $(PTARGET).c

$(PTARGET): $(PTARGET).o logging.o main.c
	$(CC) -o $@ $^ $(CFLAGS)

$(TTARGET): $(TTARGET).c $(PTARGET).o logging.o $(HEADERS)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	$(RM) *.o $(PTARGET) $(TTARGET)

