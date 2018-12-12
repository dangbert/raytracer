CC = g++
#CFLAGS = -g -Wall -Wextra -Wpedantic -pedantic-errors -O2
CFLAGS = -g -O2
#CFLAGS = -g

NAME = seamcarving
#OBJS = main.o io.o
DEPS = *.h
INCLUDE = -I../common/ -I/usr/include/eigen3/ -I/afs/umbc.edu/users/a/d/adamb/pub/
LIB = 


# build target executable
$(NAME): code.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $< -o $(NAME)

#$(NAME): $(OBJS)
#	$(CC) -o $@ $^ $(LIB)
#
## build object files for each cpp file
## $@ is the target, $^ are the dependencies
#%.o: %.cpp $(DEPS)
#	$(CC) $(INCLUDE) -c -o $@ $< $(CFLAGS)

clean:
	rm -f *.o $(NAME) out*.jpg out*.png

test:
	 make
	./$(NAME) inputs/house.jpg out-house_312x384.jpg 312 384 # was 512x384
	./$(NAME) inputs/house.jpg out-house_400x200.jpg 400 200 # was 512x384

	./$(NAME) inputs/up.jpg out-up_240x960.jpg 240 960       # was 640x960
	./$(NAME) inputs/up.jpg out-up_640x460.jpg 640 460       # was 640x960

	./$(NAME) inputs/dog.jpg out-dog_400x320.jpg 400 320     # was 500x420

 #./$(NAME) inputs/house.jpg out-house.jpg 412 384 # was 512x384
 #./$(NAME) inputs/up.jpg out-up.jpg 640 400    # was 640x960
 #./$(NAME) inputs/dog.jpg out-dog.jpg 400 320    # was 500x420

.PHONY: clean run implicit test
