//THIS IS ONLY TEST SCRIPT, DEDICATED TO PLAY WITH OPENGL
#include <stdio.h>
#include <stdlib.h>
#include <time.h>       /* time, for random seed */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>//For tranform matrices
#include <glm/gtc/matrix_transform.hpp>//For scaling and rotation matrices

#include "common/ShaderLoader.hpp"

using namespace glm;

//CONSTANTS
const int screenWidth=1024;//4:3
const int screenHeight=768;
const int ScreenFps = 60;
const int ScreenTicksPerFrame = 1000 / ScreenFps;

static GLfloat g_vertex_buffer_data_triangle[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	0.0f,  1.0f, 0.0f,
};

// Our vertices. Three consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
static GLfloat g_vertex_buffer_data_cube[] = {
	-1.0f,-1.0f,-1.0f, // triangle 1 : begin
	-1.0f,-1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f, // triangle 1 : end
	1.0f, 1.0f,-1.0f, // triangle 2 : begin
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f, // triangle 2 : end
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f
};
//Ine color for each vertex
//To draw Cube - 12 triangles, each 3 vertex, for each vertex 3 components(RGB)
static GLfloat g_color_buffer_data[12*3*3];

//THis is not going to work anyway, just testing
// Two UV coordinatesfor each vertex. They were created with Blender. You'll learn shortly how to do this yourself.
static GLfloat g_uv_buffer_data[] = {
    0.000059f, 1.0f-0.000004f,
    0.000103f, 1.0f-0.336048f,
    0.335973f, 1.0f-0.335903f,
    1.000023f, 1.0f-0.000013f,
    0.667979f, 1.0f-0.335851f,
    0.999958f, 1.0f-0.336064f,
    0.667979f, 1.0f-0.335851f,
    0.336024f, 1.0f-0.671877f,
    0.667969f, 1.0f-0.671889f,
    1.000023f, 1.0f-0.000013f,
    0.668104f, 1.0f-0.000013f,
    0.667979f, 1.0f-0.335851f,
    0.000059f, 1.0f-0.000004f,
    0.335973f, 1.0f-0.335903f,
    0.336098f, 1.0f-0.000071f,
    0.667979f, 1.0f-0.335851f,
    0.335973f, 1.0f-0.335903f,
    0.336024f, 1.0f-0.671877f,
    1.000004f, 1.0f-0.671847f,
    0.999958f, 1.0f-0.336064f,
    0.667979f, 1.0f-0.335851f,
    0.668104f, 1.0f-0.000013f,
    0.335973f, 1.0f-0.335903f,
    0.667979f, 1.0f-0.335851f,
    0.335973f, 1.0f-0.335903f,
    0.668104f, 1.0f-0.000013f,
    0.336098f, 1.0f-0.000071f,
    0.000103f, 1.0f-0.336048f,
    0.000004f, 1.0f-0.671870f,
    0.336024f, 1.0f-0.671877f,
    0.000103f, 1.0f-0.336048f,
    0.336024f, 1.0f-0.671877f,
    0.335973f, 1.0f-0.335903f,
    0.667969f, 1.0f-0.671889f,
    1.000004f, 1.0f-0.671847f,
    0.667979f, 1.0f-0.335851f
};

//GLOBALS
SDL_Window* mainWindow;
SDL_GLContext glContext;

int xpos, ypos;//mouse coords
//3D camera settings
glm::mat4 FPSViewMatrix;
glm::mat4 FPSProjectionMatrix;
// Initial camera position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 5 );
// horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// vertical angle : 0, look at the horizon
float verticalAngle = 0.0f;
// Initial Field of View
const float initialFoV = 45.0f;
glm::vec3 direction;
glm::vec3 right;
glm::vec3 up;

bool forwardPressed = false;
bool backPressed = false;
bool leftPressed = false;
bool rightPressed = false;

float speed = 0.5f; // 0.5 units / second
float mouseSpeed = 0.0001f;

//FUNCTIONS
bool initSystems();
GLuint loadShaders(const char* vertexFilePath, const char* fragmentFilePath);
void renderTriangles(GLuint count, GLuint vertexBuffer, GLuint colorBuffer = 0, GLuint textureBuffer = 0);

void computeMatricesFromInputs();
bool loadTexture(GLuint* texture, const char * filePath);
//bool loadTexture(GLuint* texture, const char * filePath, GLuint width = 0, GLuint height = 0);

GLuint createVAO() {
	GLuint vaoID;
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);
	return vaoID;
}

GLuint createVBO() {
	GLuint vboID;
	glGenBuffers(1, &vboID);
	return vboID;
}

void loadDataIntoVBO(GLuint vboID, GLfloat* data, int dataLength, GLenum usage) {
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, dataLength * sizeof(GLfloat), data, usage);
}

void configureVBO(int vertexAttributeID, GLuint vboID, int dataPerVertex) {
	glEnableVertexAttribArray(vertexAttributeID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glVertexAttribPointer(vertexAttributeID, dataPerVertex, GL_FLOAT, GL_FALSE, 0, (void*)0);
}


int main(int argc, char *argv[]){

	//Init random seed
	//For truly random numbers, Perlin Noise?
	srand (static_cast <unsigned> (time(0)));
	//Generate random colors
	for (int v = 0; v < 12*3 ; v++) {
		g_color_buffer_data[3*v+0] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		g_color_buffer_data[3*v+1] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		g_color_buffer_data[3*v+2] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	}

	if (!initSystems()) {
		return -1;
	}

	//IMPORTANT! myVector(x, y, z, w);//w == 1-point, w == 0 - direction
	//Transformation matrices flow:
	//	Model coords -> Model Matrix
	//	Worlds coords -> View Matrix
	//	Camera coords -> Projection Matrix
	//	Homogeneous Coords
	//REMEMBER: matrix multiplication order does matter!
	//Most common order for game characters
	//TransformedVector = TranslationMatrix * RotationMatrix * ScaleMatrix * OriginalVector;

	//Do this once window is created(And GL Context too) and before any other OpenGL calls
	GLuint VertexArrayID = createVAO();

	// Create and compile our GLSL program from the shaders
	ShaderLoader shaderProgram = ShaderLoader("/Users/madvi11ain/OpenGLTest/shader.vert", "/Users/madvi11ain/OpenGLTest/shader.frag");
	shaderProgram.loadShaders();
	GLuint shaderProgramID = shaderProgram.getShaderProgram();

	GLuint triangleVertexBuffer = createVBO();
	loadDataIntoVBO(triangleVertexBuffer, g_vertex_buffer_data_triangle, 9, GL_STATIC_DRAW);

	GLuint cubeVertexBuffer = createVBO();
	loadDataIntoVBO(cubeVertexBuffer, g_vertex_buffer_data_cube, 3 * 3 * 12, GL_STATIC_DRAW);

	//color vertex for cube
	GLuint colorbuffer = createVBO();
	loadDataIntoVBO(colorbuffer, g_color_buffer_data, 3 * 3 * 12, GL_STATIC_DRAW);

	//Load texture
	GLuint textureID;
	if(!loadTexture(&textureID, "./brick_wall.png")) {
		std::cout << "Couldn't load texture..\n";
	}

	//color vertex for cube
	GLuint texturebuffer = createVBO();
	loadDataIntoVBO(texturebuffer, g_uv_buffer_data, 3 * 3 * 12, GL_STATIC_DRAW);

	// Get a handle for our "MVP" uniform
	GLuint matrixID = glGetUniformLocation(shaderProgramID, "MVP");

	//FIrst clear screen to black
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(mainWindow);

	bool isRunning = true;
	//Main loop
	while(isRunning) {

		Uint32 time = SDL_GetTicks();

		// Enable depth test
		glEnable(GL_DEPTH_TEST);
		// Accept fragment if it closer to the camera than the former one
		glDepthFunc(GL_LESS);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Make colors change each frame randomly
		for (int v = 0; v < 12*3 ; v++) {
			g_color_buffer_data[3*v+0] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			g_color_buffer_data[3*v+1] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			g_color_buffer_data[3*v+2] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		}
		//load updated color data
		loadDataIntoVBO(colorbuffer, g_color_buffer_data, 3 * 3 * 12, GL_DYNAMIC_DRAW);

		glUseProgram(shaderProgramID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = FPSProjectionMatrix * FPSViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, in the "MVP" uniform
		glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);

		//renderTriangles(12, cubeVertexBuffer, 0, texturebuffer);
		renderTriangles(12, cubeVertexBuffer, colorbuffer);

		glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);

		renderTriangles(3, triangleVertexBuffer);

		SDL_GL_SwapWindow(mainWindow);

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				isRunning = false;
			if( event.type == SDL_MOUSEMOTION )
			{
				//Get the mouse offsets
				xpos = event.motion.x;
				ypos = event.motion.y;
			}
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						isRunning = false;
						break;
					case SDLK_g:
						// Cover with green and update
						glClearColor(0.0, 1.0, 0.0, 1.0);
						glClear(GL_COLOR_BUFFER_BIT);
						SDL_GL_SwapWindow(mainWindow);
						break;
					case SDLK_t:
						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
						renderTriangles(3, triangleVertexBuffer);
						SDL_GL_SwapWindow(mainWindow);
						break;
					case SDLK_w:
						forwardPressed = true;
						break;
					case SDLK_s:
						backPressed = true;
						break;
					case SDLK_a:
						leftPressed = true;
						break;
					case SDLK_d:
						rightPressed = true;
						break;
					default:
						break;
				}
			}
			if (event.type == SDL_KEYUP)
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_w:
						forwardPressed = false;
						break;
					case SDLK_s:
						backPressed = false;
						break;
					case SDLK_a:
						leftPressed = false;
						break;
					case SDLK_d:
						rightPressed = false;
						break;
					default:
						break;
				}
			}
		}

		//Limit FPS
		if(ScreenTicksPerFrame>(SDL_GetTicks()-time))
		{
			SDL_Delay(ScreenTicksPerFrame-(SDL_GetTicks()-time)); //SDL_Delay pauses the execution.
		}
	}

	return 0;
}

//We only pass VBOs which change on every frame
void renderTriangles(GLuint count, GLuint vertexBuffer, GLuint colorBuffer, GLuint textureBuffer) {

	if (vertexBuffer) {
		configureVBO(0, vertexBuffer, 3);
		//glDisableVertexAttribArray(0);
	}
	if (textureBuffer) {
		configureVBO(1, textureBuffer, 2);
		//glDisableVertexAttribArray(1);
	}
	if (colorBuffer) {
		// 2nd attribute buffer : colors
		configureVBO(2, colorBuffer, 3);
		//glDisableVertexAttribArray(2);
	}
	//Draw triangles
	glDrawArrays(GL_TRIANGLES, 0, count*3); // 12*3 indices starting at 0 -> 12 triangles -> 6 squares
}

//Calculate Projection matrices from keyboard and mouse inputs
void computeMatricesFromInputs() {

	static Uint32 lastTime = SDL_GetTicks();//this is called only once, first time function is called

	//Get difference between current and last frame
	Uint32 currentTime = SDL_GetTicks();
	float deltaTime = float(currentTime = lastTime);
	// Reset mouse position for next frame
	SDL_WarpMouseInWindow(mainWindow, screenWidth/2, screenHeight/2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * deltaTime * float(screenWidth/2 - xpos );
	verticalAngle   += mouseSpeed * deltaTime * float( screenHeight/2 - ypos );

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	direction = glm::vec3(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	// Right vector
	right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f),
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	// Up vector : perpendicular to both direction and right
	up = glm::cross( right, direction );

	// Move forward
	if (forwardPressed){
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (backPressed){
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (rightPressed){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (leftPressed){
		position -= right * deltaTime * speed;
	}

	// Projection matrix : 45&deg; Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	FPSProjectionMatrix = glm::perspective(glm::radians(initialFoV), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	FPSViewMatrix =  glm::lookAt(
			position,           // Camera is here
			position+direction, // and looks here : at the same position, plus "direction"
			up                  // Head is up (set to 0,-1,0 to look upside-down)
			);

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

bool loadTexture(GLuint* texture, const char * filePath/*, GLuint& width, GLuint& height*/) {

	//TODO: Use compressed textures, 20% increase in performance

	// You should probably use CSurface::OnLoad ... ;)
	//-- and make sure the Surface pointer is good!
	SDL_Surface* surface = IMG_Load(filePath);

	if(!surface) {
		return false;
	}

	 if(surface){
        // Check that the image’s width is a power of 2
        if ( (surface->w & (surface->w - 1)) != 0 ) {
			std::cout << "Warning: image.bmp’s width is not a power of 2\n" << std::endl;
        }

        // Also check if the height is a power of 2
        if ( (surface->h & (surface->h - 1)) != 0 ) {
			std::cout << "Warning: image.bmp’s height is not a power of 2\n" << std::endl;
        }
	 }

	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);

	int Mode = GL_RGB;

	if(surface->format->BytesPerPixel == 4) {
		Mode = GL_RGBA;
	}

	// Give the image to OpenGL
	std::cout << surface->w << std::endl;
	std::cout << surface->h << std::endl;
	glTexImage2D(GL_TEXTURE_2D, 0, Mode, surface->w, surface->h, 0, Mode, GL_UNSIGNED_BYTE, surface->pixels);

	//Also GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return true;
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
	glewExperimental = GL_TRUE;
	glewInit();

	return true;
}
