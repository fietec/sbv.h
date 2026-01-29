TARGET = example
SRCS   = sbv.h example.c
CFLAGS = -Wall -Wextra --std=c99

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)
