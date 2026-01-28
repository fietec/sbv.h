TARGET = example
SRCS   = sbv.h example.c
CFLAGS = -Wall -Wextra --std=c99

$(TARGET): $(SRCS)
	cc $(CFLAGS) -o $(TARGET) $(SRCS)
