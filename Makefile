TARGET = my_tar
SRCS = src/my_tar.c src/helper.c src/getopt.c src/parser.c src/build_archive.c src/memory.c src/tar_header.c src/read_and_update_archive.c src/extract_archive.c
OBJS = $(SRCS:.c=.o)
CC = gcc
CFLAGS = -Wall -Wextra -Werror 
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o

fclean: clean
	rm -f $(TARGET)

re: fclean all