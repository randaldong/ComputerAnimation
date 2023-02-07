#pragma once

#include <stdio.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "core.h"

class Shader
{
public:
	GLuint shaderID;
	enum ShaderType{eVertex, eFragment};

	Shader(const char* shaderFile, ShaderType shaderType);
	~Shader();

};

class ShaderProgram
{
public:
	GLuint programID;
	// Shader array that contains vertex shader & fragment shader;
	// shaders[0] for vertex shader; shaders[1] for fragment shader.
	Shader* shaders[2];

	ShaderProgram(const char* shaderFile);
	~ShaderProgram();
};