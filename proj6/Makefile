CC = g++
#CFLAGS = -g -Wall -Wextra -Wpedantic -pedantic-errors -O2
CFLAGS = -g -O2

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
	 ./$(NAME) inputs/house.jpg out-house.jpg 512 384  # was 512x384
	 ./$(NAME) inputs/up.jpg out-up.jpg 640 960        # was 640x960
	 ./$(NAME) inputs/dog.jpg out-dog.jpg 500 420      # was 500x420

 #./$(NAME) inputs/house.jpg out-house.jpg 412 384 # was 512x384
 #./$(NAME) inputs/up.jpg out-up.jpg 640 400    # was 640x960
 #./$(NAME) inputs/dog.jpg out-dog.jpg 400 320    # was 500x420

.PHONY: clean run implicit test
