CC          = g++ 
CFLAGS = -g -Wall -Wextra -Wpedantic -pedantic-errors -O2

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
	./fishtank sample.in

#-----------------------------------------
#-----------------------------------------

viewer: viewer.cpp
	$(CC) $(CFLAGS) $(INCS) $(LDOPTS) $(GL_LIB) -c $< -o $@

fishtank: fishtank.o $(OBJECTS)
	$(CC) $(CFLAGS) $(INCS) $(LDOPTS) $^ -o $@

fishtank.o: fishtank.cpp
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

Boids.o: Boids.cpp Boids.h
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@
#-----------------------------------------
#-----------------------------------------
