#SOURCES
OBJS= main.cpp common/*.cpp

#COMPILER
CC= clang++#or g++
#COMPILER FLAGS
CF=-Wall -g -std=c++11 -std=c++14 -fexceptions

#LINKER FLAGS
#For Mac OS X: brew install sdl2; brew install sdl2_image; brew install glew; brew install sdl2_ttf
LDF= -framework OpenGL -framework SDL2 -framework SDL2_image -lSDL2main -lglew -llua #-lSDL2 -lBox2D -framework SDL2_ttf

#Includes here: /usr/local/Cellar/glm/0.9.8.5/include/
INCLUDE_PATHS = -I/usr/local/include -I/opt/X11/include #-I/Library/Frameworks/SDL2.framework/Headers
LIBRARY_PATHS = -L/usr/local/lib -I/opt/X11/lib

OBJ_NAME= main

all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(CF) $(LDF) -o $(OBJ_NAME)

