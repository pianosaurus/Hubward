BIN=../minedraft
CC=g++
CFLAGS=-O3 -fopenmp -std=c++0x -pipe
LIBRARIES=-lz -lpng
DEFINES=
OBJECTS=main.o output.o level.o colours.o nbt.o nbttags.o chunk.o	\
	intstring.o options.o pixel.o image.o pvector.o renderer.o	\
	render_contour.o

.PHONY : clean binary version.h

binary : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN) $^ $(LIBRARIES) $(DEFINES)

main.o : main.cpp version.h
	$(CC) $(CFLAGS) -c -o $@ $< $(DEFINES)

colours.o : colours.cpp renderer.hpp
	$(CC) $(CFLAGS) -c -o $@ $< $(DEFINES)

%.o : %.cpp %.hpp
	$(CC) $(CFLAGS) -c -o $@ $< $(DEFINES)

version.h :
	sed -e 's/#define \+APPVERSION \+"[^"]*"/#define APPVERSION "'`git describe`'"/' version.h.base > version.h

clean :
	-rm -f $(OBJECTS)
