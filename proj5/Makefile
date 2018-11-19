CC          = g++ 
CFLAGS = -g -Wall -Wextra -Wpedantic -pedantic-errors

#-----------------------------------------
#Optimization ----------------------------
OPT   = -O3 -Wno-deprecated

#GL_LIB = -framework OpenGL -framework GLUT -framework foundation
GL_LIB = -lGL -lglut -lGLU

#-----------------------------------------

TARGETS = fishtank viewer
OBJECTS = Boids.o

#-----------------------------------------

LIBS = 
INCS = -I../common -I/usr/local/include/eigen3 -I/usr/include/eigen3

CCOPTS = $(OPT) $(INCS) 
LDOPTS = $(OPT) $(INCS) $(LIBS) 

#-----------------------------------------
#-----------------------------------------

.PHONY: run clean

default: $(TARGETS)

clean:
	/bin/rm -f *.o $(TARGETS)

run:
	make && ./fishtank sample.in

test:
	make && rm -f out && ./fishtank sample.in > out && ./teacher/viewer out

#-----------------------------------------
#-----------------------------------------

viewer: viewer.cpp
	$(CC) $(INCS) $(LDOPTS) $(GL_LIB) -c $< -o $@
# (ignore warnings in viewer cause I didn't write it)
#$(CC) $(CFLAGS) $(INCS) $(LDOPTS) $(GL_LIB) -c $< -o $@

fishtank: fishtank.o $(OBJECTS)
	$(CC) $(CFLAGS) $(INCS) $(LDOPTS) $^ -o $@

fishtank.o: fishtank.cpp
	$(CC) $(CFLAGS) $(LDOPTS) -c $< -o $@

Boids.o: Boids.cpp Boids.h
	$(CC) $(CFLAGS) $(LDOPTS) -c $< -o $@
#-----------------------------------------
#-----------------------------------------
