# https://developers.redhat.com/blog/2018/03/21/compiler-and-linker-flags-gcc/
CC = g++
CFLAGS = -g -Wall -Wextra -Wpedantic -pedantic-errors -O2
#-std=c++98

NAME = proj1
OBJS = main.o RayTracer.o SettingsNFF.o Ray.o
DEPS = *.h
INCLUDE = -I../../common


# build target executable
$(NAME): $(OBJS)
	$(CC) -o $@ $^

# build object files for each cpp file
# $@ is the target, $^ are the dependencies
%.o: %.cpp $(DEPS)
	$(CC) $(INCLUDE) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o $(NAME)

.PHONY: run
run:
	make -B && ./$(NAME)
