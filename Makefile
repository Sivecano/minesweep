CXXFLAGS = -Ofast
LDLIBS := -lSDL2 -lSDL2_net -lSDL2_image
CC = g++

all: cli client server main server_headless

cli : field.o

client: field.o rfield.o gui.o

server: field.o gui.o hfield.o

main: field.o gui.o

server_headless: hfield.o field.o

gui.o : gui.config.h

#	g++ -o ms $< $(LDFLAGS) 

WINCC := x86_64-w64-mingw32-g++
WINCXXFLAGS := $(CXXFLAGS) -I/usr/x86_64-w64-mingw32/include -L/usr/x86_64-w64-mingw32/lib
WINLDLIBS := -static-libgcc -static-libstdc++ -lSDL2_net.dll
WINSDL := -lmingw32 -lSDL2main -lSDL2 -lSDL2_image.dll

%.wo : %.cpp
	$(WINCC) -c $(WINCXXFLAGS) $^ -o $@ $(WINLDLIBS)

%.exe :  %.wo 
	$(WINCC) $(WINCXXFLAGS) -mwindows $^ -o win/$@ $(WINLDLIBS) $(WINSDL)

win :
	mkdir win
	cp tiles.bmp win
	cp /usr/x86_64-w64-mingw32/bin/SDL2.dll win 
	cp /usr/x86_64-w64-mingw32/bin/SDL2_image.dll win 
	cp /usr/x86_64-w64-mingw32/bin/SDL2_net.dll win 
	cp /usr/x86_64-w64-mingw32/bin/libwinpthread-1.dll win 

winall : win cli.exe client.exe server.exe main.exe server_headless.exe

client.exe : field.wo rfield.wo gui.wo

server.exe : field.wo gui.wo hfield.wo

main.exe : field.wo gui.wo

server_headless.exe : hfield.wo field.wo server_headless.wo
	$(WINCC) $(WINCXXFLAGS) -mconsole $^ -o win/$@ $(WINLDLIBS)

cli.exe : field.wo cli.wo
	$(WINCC) $(WINCXXFLAGS) -mconsole $^ -o win/$@ $(WINLDLIBS)
