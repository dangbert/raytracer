CC = g++
CFLAGS = -g -Wall -Wextra -Wpedantic -pedantic-errors -O2

NAME = proj2
INCLUDE = -I../common -I/usr/include/eigen3/


# build target executable
$(NAME): main.cpp
	pushd ../common && make && popd && $(CC) -o $@ $^ $(INCLUDE) -L../common -lgraphics && echo "done"

.PHONY: clean
clean:
	rm -f *.o $(NAME) out.ppm && pushd ../common && make clean && popd

.PHONY: run
run:
	make && ./$(NAME) inputs/teapot-3.nff && display out.ppm
