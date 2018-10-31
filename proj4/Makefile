# https://developers.redhat.com/blog/2018/03/21/compiler-and-linker-flags-gcc/
CC = g++
CFLAGS = -g -Wall -Wextra -Wpedantic -pedantic-errors -O2

NAME = smoothing
OBJS = main.o io.o
DEPS = *.h
INCLUDE = -I../common/ -I/usr/include/eigen3/


# build target executable
$(NAME): $(OBJS)
	$(CC) -o $@ $^

# build object files for each cpp file
# $@ is the target, $^ are the dependencies
%.o: %.cpp $(DEPS)
	$(CC) $(INCLUDE) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o $(NAME) out.obj

.PHONY: run
run:
	./$(NAME) inputs/bunny.obj out.obj 1 1 50
