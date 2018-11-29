#pragma once

#ifndef SHADER_H
#define SHADER_H

#include <GLAD\glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

using namespace std;


class Shader
{
public:
	//Program ID
	unsigned int ID;

	//Constructor reads and builds the shader
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	
	/// <summary>
	/// Name, Position, Ambient, Diffuse, Specular, Constant, Linear, Quadratic
	/// <seealso cref="Shader::LightSettings"/>  
	/// </summary>  
	struct LightSettings
	{
		string name;
		glm::vec3 position;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float constant;
		float linear;
		float quadratic;
	};

	//Use|Activate the shader
	void StartPipelineProgram();
	void StartPipelineProgram(glm::mat4 projection, glm::mat4 view, glm::mat4 model);

	//Utility uniform functions (Const is used at the end to make sure the object (*this) isn't modified when called or by the methods))
	void setBool(const string &name, bool value) const;
	void setInt(const string &name, int value) const;
	void setFloat(const string &name, float value) const;
	void setVec3(const string &name, glm::vec3 value) const;
	void setMat4(const string &name, glm::mat4 value) const;
	void setPointLight(LightSettings settings);

	
	~Shader();
};

#endif 
