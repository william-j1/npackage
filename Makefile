CC = gcc
CFLAGS = -Wall -Wextra -I.
SOURCES = main.c npackage.c utils.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = np.exe
all: $(EXECUTABLE)
$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $^
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
.PHONY: all clean
