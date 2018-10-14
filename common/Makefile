CC = g++
CFLAGS = -g -Wall -Wextra -Wpedantic -pedantic-errors -O2

OUT = libgraphics.a
OBJS = Tracer.o Rasterizer.o Settings.o Surfaces.o Test.o
DEPS = *.h
INCLUDE = -I. -I/usr/include/eigen3/

# build static library file (.a)
$(OUT): $(OBJS)
	ar ru $@ $^

# build object files for each cpp file
# $@ is the target, $^ are the dependencies
%.o: %.cpp $(DEPS)
	$(CC) $(INCLUDE) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o $(OUT)
