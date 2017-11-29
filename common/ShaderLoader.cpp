#include "ShaderLoader.hpp"
#include <fstream>
#include <vector>

ShaderLoader::ShaderLoader(std::string vertexShaderPath, std::string fragmentShaderPath)
	: vertexShaderPath(vertexShaderPath), fragmentShaderPath(fragmentShaderPath)
{
	// Create the shaders
	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

}

ShaderLoader::~ShaderLoader()
{
}

void ShaderLoader::loadShaders()
{
	// Read the Vertex Shader code from the file
	std::string vertexShaderCode;
	std::ifstream vertexShaderStream(vertexShaderPath, std::ios::in);
	if(vertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(vertexShaderStream, Line))
			vertexShaderCode += "\n" + Line;
		vertexShaderStream.close();
	}else{
		//printf("Impossible to open %s. Are you in the right directory ?\n", vertexShaderPath);
		//getchar();
		//return 0;
	}

	// Read the Fragment Shader code from the file
	std::string fragmentShaderCode;
	std::ifstream fragmentShaderStream(fragmentShaderPath, std::ios::in);
	if(fragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(fragmentShaderStream, Line))
			fragmentShaderCode += "\n" + Line;
		fragmentShaderStream.close();
	}

	GLint result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	//printf("Compiling shader : %s\n", vertexFilePath);
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
	//printf("Compiling shader : %s\n", fragmentFilePath);
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
	//printf("Linking program\n");
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShaderID);
	glAttachShader(shaderProgramID, fragmentShaderID);
	glLinkProgram(shaderProgramID);

	// Check the program
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &result);
	glGetProgramiv(shaderProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> programErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(shaderProgramID, InfoLogLength, NULL, &programErrorMessage[0]);
		//printf("%s\n", &programErrorMessage[0]);
	}

	glDetachShader(shaderProgramID, vertexShaderID);
	glDetachShader(shaderProgramID, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	//return programID;
}
