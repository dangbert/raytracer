CC = g++
CFLAGS = -std=c++98 -g -Wall -Wextra -Wpedantic -pedantic-errors

NAME = proj1
OBJS = main.o RayTracer.o SettingsNFF.o
DEPS = *.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(NAME): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)


.PHONY: clean
clean:
	rm -f *.o $(NAME)

.PHONY: run
run:
	make -B && ./$(NAME)
