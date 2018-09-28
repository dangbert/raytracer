# https://developers.redhat.com/blog/2018/03/21/compiler-and-linker-flags-gcc/
CC = g++
CFLAGS = -g -Wall -Wextra -Wpedantic -pedantic-errors -O2

NAME = proj2
OBJS = main.o Tracer.o Settings.o Surfaces.o
DEPS = *.h
INCLUDE = -I../common -I/usr/include/eigen3/


# build target executable
$(NAME): $(OBJS)
	$(CC) -o $@ $^

# build object files for each cpp file
# $@ is the target, $^ are the dependencies
%.o: %.cpp $(DEPS)
	$(CC) $(INCLUDE) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o $(NAME) out.ppm

.PHONY: run
run:
	make && ./$(NAME) ../proj1/inputs/rotated-tetra-3.nff
