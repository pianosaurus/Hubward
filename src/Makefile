BIN=../minedraft
CC=g++
CFLAGS=-O3 -fopenmp -std=c++0x -pipe
LIBRARIES=-lz -lpng
DEFINES=
OBJECTS=main.o output.o level.o colours.o nbt.o nbttags.o chunk.o	\
	intstring.o options.o pixel.o image.o pvector.o renderer.o	\
	render_contour.o

.PHONY : clean binary

binary : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN) $^ $(LIBRARIES) $(DEFINES)

main.o : main.cpp
	$(CC) $(CFLAGS) -c -o $@ $< $(DEFINES)

colours.o : colours.cpp renderer.hpp
	$(CC) $(CFLAGS) -c -o $@ $< $(DEFINES)

%.o : %.cpp %.hpp
	$(CC) $(CFLAGS) -c -o $@ $< $(DEFINES)

clean :
	-rm -f $(OBJECTS)
