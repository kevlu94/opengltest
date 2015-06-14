CC = g++

CFLAGS = -w

INCLUDES = -I. -I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include -I/usr/local/include -I/usr/include 

LFLAGS = -L/Applications/XCode.app -L/usr/local/lib -L/glew-1.12.0/lib

LIBS = -lSOIL -lGLEW -lglfw3

FFLAGS = 

FRAMEWORKS = -framework CoreGraphics -framework CoreFoundation -framework OpenGL -framework CoreVideo -framework IOKit -framework AppKit

all: test proc

test: common/*.cpp camera.cpp model.cpp scene.cpp main.cpp
	$(CC) $(CFLAGS) $(INCLUDES) $(LFLAGS) $(LIBS) $(FFLAGS) $(FRAMEWORKS) common/*.cpp camera.cpp model.cpp scene.cpp main.cpp -o test

proc: Kabsch.cpp proc-super.cpp
	$(CC) $(CFLAGS) -I/usr/local/include Kabsch.cpp proc-super.cpp -o proc

run:
	./test faces/0.obj faces/0.jpg

clean:
	rm test proc