CC = g++

CFLAGS = -w

INCLUDES = -I. -I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include -I/usr/local/include -I/usr/include 

LFLAGS = -L/Applications/XCode.app -L/usr/local/lib -L/glew-1.12.0/lib

LIBS = -lSOIL -lGLEW -lglfw3

FFLAGS = 

FRAMEWORKS = -framework CoreGraphics -framework CoreFoundation -framework OpenGL -framework CoreVideo -framework IOKit -framework AppKit

all: test

test: common/*.cpp *.cpp
	$(CC) $(CFLAGS) $(INCLUDES) $(LFLAGS) $(LIBS) $(FFLAGS) $(FRAMEWORKS) common/*.cpp *.cpp -o test

run:
	./test faces/0.obj faces/0.jpg

clean:
	rm test