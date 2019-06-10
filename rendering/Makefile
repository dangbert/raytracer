CC = g++
CFLAGS = -g -Wall -Wextra -Wpedantic -pedantic-errors -O2

RAY_OUT = raytracer
RAY_DRIVER = main-raytracer.cpp
OBJS = Rasterizer.o Tracer.o Settings.o Surfaces.o Test.o

RAS_OUT = rasterizer
RAS_DRIVER = main-rasterizer.cpp

DEPS = *.h
INCLUDE = -I/usr/include/eigen3/ -I../libraries

# build executables:
all: $(RAY_OUT) $(RAS_OUT)

$(RAY_OUT): $(RAY_DRIVER) $(OBJS)
	$(CC) $(INCLUDE) $^ -o $@
$(RAS_OUT): $(RAS_DRIVER) $(OBJS)
	$(CC) $(INCLUDE) $^ -o $@

# build object files for each cpp file
# $@ is the target, $^ are the dependencies
# (perhaps not the most efficient)
%.o: %.cpp $(DEPS)
	$(CC) $(INCLUDE) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o $(RAS_OUT) $(RAY_OUT)


##### old makefile system:####
### build static library file (.a)
### (of common classes shared by raytracer and rasterizer)
#libgraphics.a: $(OBJS)
#	ar ru $@ $^

### compile rasterizer/raytracer seperately
#$(NAME): main.cpp
#	pushd ../common && make && popd && $(CC) -o $@ $^ $(INCLUDE) -L../common -lgraphics && echo "done"
##### old makefile system:####
##############################
