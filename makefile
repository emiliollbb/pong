#CC specifies which compiler we're using 
CC = gcc 

#COMPILER_FLAGS specifies the additional compilation options we're using 
# -w suppresses all warnings 
COMPILER_FLAGS = -w 

#LINKER_FLAGS specifies the libraries we're linking against 
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf

#This is the target that compiles our executable 

all: main pong1p pong2p test test2

main : main.c
	$(CC) main.c $(COMPILER_FLAGS) $(LINKER_FLAGS) -o main

pong1p: pong1p.c
	$(CC) pong1p.c $(COMPILER_FLAGS) $(LINKER_FLAGS) -o pong1p 

pong2p: pong2p.c
	$(CC) pong2p.c $(COMPILER_FLAGS) $(LINKER_FLAGS) -o pong2p 

test: test.c
	$(CC) test.c $(COMPILER_FLAGS) $(LINKER_FLAGS) -o test 


test2: test2.c
	$(CC) test2.c $(COMPILER_FLAGS) $(LINKER_FLAGS) -o test2 
