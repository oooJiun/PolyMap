CC = g++
GLFLAGS = -lGL -lGLU -lglut
SRC = AND.cpp

.PHONY: all clean
all:
	$(CC) $(SRC) $(GLFLAGS) -o output
	./output

clean:
	rm output