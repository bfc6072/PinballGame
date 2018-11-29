#include "Shader.h"



Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	/*
	 * Part 1!
	 * Retrieve the vertex/fragment source code from filePath
	 */
	string vertexCode;
	string fragmentCode;
	ifstream vShaderFile;
	ifstream fShaderFile;

	//Ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

	//Try to read in the shader files
	try
	{
		//open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		stringstream vShaderStream, fShaderStream;

		//Read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf(); //returns a pointer to the stream buffer object currently associated with the stream.
		fShaderStream << fShaderFile.rdbuf();

		//close file handlers
		vShaderFile.close();
		fShaderFile.close();

		//convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (ifstream::failure e)
	{
		cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();


	/*
	* Part 2!
	* Compile shaders
	*/
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	//Vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER); //Create vertex shader on pipeline returning reference int
	glShaderSource(vertex, 1, &vShaderCode, NULL);	//Replace the source code in a shader object with the one we wrote (Setting it)
	glCompileShader(vertex); //Compiling the shader

	//Print compile errors
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
	}

	//Fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
	}

	//Shader program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	//Print Linking errors if any
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
	}

	//Delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::StartPipelineProgram()
{
	glUseProgram(ID);

}

void Shader::StartPipelineProgram(glm::mat4 projection, glm::mat4 view, glm::mat4 model)
{
	glUseProgram(ID);
	setMat4("projection", projection);
	setMat4("view", view);
	setMat4("model", model);
}

void Shader::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);

}
void Shader::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const string &name, glm::vec3 value) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}

void Shader::setMat4(const string &name, glm::mat4 value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, false, value_ptr(value));
}

void Shader::setPointLight(Shader::LightSettings settings)
{
	setVec3(settings.name + ".position", settings.position);
	setVec3(settings.name + ".ambient", settings.ambient);
	setVec3(settings.name + ".diffuse", settings.diffuse);
	setVec3(settings.name + ".specular", settings.specular);
	setFloat(settings.name + ".constant", settings.constant);
	setFloat(settings.name + ".linear", settings.linear);
	setFloat(settings.name + ".quadratic", settings.quadratic);
}

Shader::~Shader()
{
}
