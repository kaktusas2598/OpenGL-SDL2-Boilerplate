#pragma once

#include <GL/glew.h>
#include <iostream>

class ShaderLoader
{
	public:
		ShaderLoader(std::string vertexShaderPath, std::string fragmentShaderPath);
		~ShaderLoader();

		void loadShaders();

		GLuint getShaderProgram() { return shaderProgramID; }
	private:

		GLuint shaderProgramID;

		GLuint vertexShaderID;
		GLuint fragmentShaderID;

		std::string vertexShaderPath;
		std::string fragmentShaderPath;
};
