CXXFLAGS := -Ofast
LDFLAGS := -lSDL2 -lSDL2_net -lSDL2_image
CC = g++

cli : field.o

main: field.o rfield.o hfield.o

#	g++ -o ms $< $(LDFLAGS) 
