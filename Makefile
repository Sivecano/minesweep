CXXFLAGS := -Ofast
LDFLAGS := -lSDL2 -lSDL2_net -lSDL2_image
CC = g++

all: cli client server main

cli : field.o

client: field.o rfield.o gui.o

server: field.o gui.o hfield.o

main: field.o gui.o

#	g++ -o ms $< $(LDFLAGS) 
