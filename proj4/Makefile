# https://developers.redhat.com/blog/2018/03/21/compiler-and-linker-flags-gcc/
CC = g++
CFLAGS = -g -Wall -Wextra -Wpedantic -pedantic-errors -O2

NAME = smoothing
OBJS = main.o io.o
DEPS = *.h
INCLUDE = -I../common/ -I/usr/include/eigen3/
# note on fedora I had to: sudo ln -s /usr/lib64/libblas.so.3.8.0 /usr/lib64/libblas.so
# (because libblas.so did not exist for some reason)
LIB = -L/usr/lib64 -lblas


# build target executable
$(NAME): $(OBJS)
	$(CC) -o $@ $^ $(LIB)

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

.PHONY: implicit
implicit:
	./smoothing -i 10e-10 inputs/bunny.obj out_implicit.obj 1 1 50
