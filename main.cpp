//THIS IS ONLY TEST SCRIPT, DEDICATED TO PLAY WITH OPENGL
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <vector>

#include <glm/glm.hpp>
using namespace glm;

//CONSTANTS
const int screenWidth=640;
const int screenHeight=480;

static const GLfloat g_vertex_buffer_data[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	0.0f,  1.0f, 0.0f,
};

//GLOBALS
SDL_Window* mainWindow;
SDL_GLContext glContext;

//FUNCTIONS
bool initSystems();
GLuint loadShaders(const char* vertexFilePath, const char* fragmentFilePath);
void renderTriangle(GLuint vertexBuffer);

int main(int argc, char *argv[]){

	if (!initSystems()) {
		return -1;
	}

	//CREATE VAO
	//Do this once window is created(And GL Context too) and before any other OpenGL calls
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	//BIND VERTICS TO VERTEX BUFFER
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	//FIrst clear screen to black
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(mainWindow);

	// Create and compile our GLSL program from the shaders
	GLuint shaderProgramID = loadShaders( "shader.vert", "shader.frag" );

	bool isRunning = true;
	//Main loop
	while(isRunning) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shaderProgramID);
		renderTriangle(vertexbuffer);
		SDL_GL_SwapWindow(mainWindow);

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				isRunning = false;
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						isRunning = false;
						break;
					case SDLK_r:
						// Cover with red and update
						glClearColor(1.0, 0.0, 0.0, 1.0);
						glClear(GL_COLOR_BUFFER_BIT);
						SDL_GL_SwapWindow(mainWindow);
						break;
					case SDLK_g:
						// Cover with green and update
						glClearColor(0.0, 1.0, 0.0, 1.0);
						glClear(GL_COLOR_BUFFER_BIT);
						SDL_GL_SwapWindow(mainWindow);
						break;
					case SDLK_b:
						// Cover with blue and update
						glClearColor(0.0, 0.0, 1.0, 1.0);
						glClear(GL_COLOR_BUFFER_BIT);
						SDL_GL_SwapWindow(mainWindow);
						break;
					case SDLK_d:
						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
						glUseProgram(shaderProgramID);
						renderTriangle(vertexbuffer);
						SDL_GL_SwapWindow(mainWindow);
						break;
					default:
						break;
				}
			}
		}

	}

	return 0;
}

void renderTriangle(GLuint vertexBuffer) {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);
	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
	glDisableVertexAttribArray(0);
}

bool initSystems() {

	//Init SDL2
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "Failed to init SDL\n";
		return false;
	}

	//Set OpenGL Attributes
	//Set OpenGL v3.3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	//Basically deprecates older code
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	//Enable double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	mainWindow = SDL_CreateWindow(
			"My Game",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			screenWidth,
			screenHeight,
			SDL_WINDOW_OPENGL
			);

	if (!mainWindow) {
		std::cout << "Failed to create SDL Window\n";
		return false;
	}

	glContext = SDL_GL_CreateContext(mainWindow);

	// Init GLEW
	// Apparently, this is needed for Apple. Thanks to Ross Vander for letting me know
	//#ifndef __APPLE__
	glewExperimental = GL_TRUE;
	glewInit();
	//#endif

	return true;
}

GLuint loadShaders(const char* vertexFilePath, const char* fragmentFilePath)
{
	// Create the shaders
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string vertexShaderCode;
	std::ifstream vertexShaderStream(vertexFilePath, std::ios::in);
	if(vertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(vertexShaderStream, Line))
			vertexShaderCode += "\n" + Line;
		vertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ?\n", vertexFilePath);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string fragmentShaderCode;
	std::ifstream fragmentShaderStream(fragmentFilePath, std::ios::in);
	if(fragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(fragmentShaderStream, Line))
			fragmentShaderCode += "\n" + Line;
		fragmentShaderStream.close();
	}

	GLint result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertexFilePath);
	char const * VertexSourcePointer = vertexShaderCode.c_str();
	glShaderSource(vertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(vertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> vertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(vertexShaderID, InfoLogLength, NULL, &vertexShaderErrorMessage[0]);
		printf("%s\n", &vertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragmentFilePath);
	char const * FragmentSourcePointer = fragmentShaderCode.c_str();
	glShaderSource(fragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(fragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> fragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(fragmentShaderID, InfoLogLength, NULL, &fragmentShaderErrorMessage[0]);
		printf("%s\n", &fragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	// Check the program
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> programErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(programID, InfoLogLength, NULL, &programErrorMessage[0]);
		printf("%s\n", &programErrorMessage[0]);
	}

	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	return programID;
}
