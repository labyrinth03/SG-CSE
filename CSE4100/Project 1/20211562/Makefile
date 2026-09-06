CC = gcc
CFLAGS = -Wall -Wextra -Werror -g

SRCS = main.c list.c hash.c bitmap.c debug.c hex_dump.c
OBJS = $(SRCS:.c=.o)
HEADERS = list.h hash.h bitmap.h debug.h hex_dump.h limits.h round.h

TARGET = testlib
PRJ_TESTER = sp_prj1_tester/prj1_tester.sh 

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)

test: $(TARGET)
	@echo "Running tests with $(PRJ_TESTER)..."
	sh $(PRJ_TESTER) ./$(TARGET)

rebuild: clean all

.PHONY: all clean test rebuild
