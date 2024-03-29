#pragma once

#include <GLES3/gl3.h> 
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

class Shader
{
public:

	void BindAttributeLocation(int index, const std::string& attribute)
	{
		// Bind attribute index 0 (coordinates) to in_Position and attribute index 1 (color) to in_Color
		// Attribute locations must be setup before calling glLinkProgram
		glBindAttribLocation(shaderProgram, index, attribute.c_str());
	}

	void UseProgram()
	{
		// Load the shader into the rendering pipeline
		glUseProgram(shaderProgram);
	}

	Shader()
	{
		Init();
	}

	~Shader()
	{
		/* Cleanup all the things we bound and allocated */
		glUseProgram(0);
		glDetachShader(shaderProgram, vertexShader);
		glDetachShader(shaderProgram, fragmentShader);

		glDeleteProgram(shaderProgram);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
	
	GLuint getshaderID()
	{
		return shaderProgram;
	}

private:

	bool Init()
	{
		shaderProgram = glCreateProgram();

		if (!LoadVertexShader("../tutorial2.vert"))
			return false;

		if (!LoadFragmentShader("../tutorial2.frag"))
			return false;

		return LinkShaders();
	}

	std::string ReadFile(const char* file)
	{
		// Open file
		std::ifstream t(file);
		std::stringstream buffer;
		std::string fileContent;

		if (t.is_open())
		{
			buffer << t.rdbuf();
			fileContent = buffer.str();
		}

		return fileContent;
	}

	bool LoadVertexShader(const std::string& filename)
	{
		std::cout << "Linking Vertex shader" << std::endl;

		// Read file as std::string 
		std::string str = ReadFile(filename.c_str());

		// c_str() gives us a const char*, but we need a non-const one
		char* src = const_cast<char*>(str.c_str());
		GLint size = (GLint)str.length();

		// Create an empty vertex shader handle
		vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader source code to OpenGL
		glShaderSource(vertexShader, 1, &src, &size);

		// Compile the vertex shader
		glCompileShader(vertexShader);

		int wasCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &wasCompiled);

		if (wasCompiled == 0)
		{
			PrintShaderCompilationErrorInfo(vertexShader);
			return false;
		}

		glAttachShader(shaderProgram, vertexShader);
		return true;
	}

	bool LoadFragmentShader(const std::string& filename)
	{
		std::cout << "Loading Fragment Shader" << std::endl;

		// Read file as std::string 
		std::string str = ReadFile(filename.c_str());

		// c_str() gives us a const char*, but we need a non-const one
		char* src = const_cast<char*>(str.c_str());
		GLint size = (GLint)str.length();

		// Create an empty vertex shader handle
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the vertex shader source code to OpenGL
		glShaderSource(fragmentShader, 1, &src, &size);

		// Compile the vertex shader
		glCompileShader(fragmentShader);

		int wasCompiled = 0;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &wasCompiled);

		if (wasCompiled == false)
		{
			PrintShaderCompilationErrorInfo(fragmentShader);
			return false;
		}

		glAttachShader(shaderProgram, fragmentShader);
		return true;
	}

	bool LinkShaders()
	{
		// Link. At this point, our shaders will be inspected/optized and the binary code generated
		// The binary code will then be uploaded to the GPU
		glLinkProgram(shaderProgram);

		// Verify that the linking succeeded
		int isLinked;
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, (int*)& isLinked);

		if (isLinked == false)
			PrintShaderLinkingError(shaderProgram);

		return isLinked != 0;
	}

	void PrintShaderLinkingError(int32_t shaderId)
	{
		std::cout << "=======================================\n";
		std::cout << "Shader linking failed : " << std::endl;

		// Find length of shader info log
		int maxLength;
		glGetProgramiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

		std::cout << "Info Length : " << maxLength << std::endl;

		// Get shader info log
		char* shaderProgramInfoLog = new char[maxLength];
		glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, shaderProgramInfoLog);

		std::cout << "Linker error message : " << shaderProgramInfoLog << std::endl;

		/* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
		/* In this simple program, we'll just leave */
		delete []shaderProgramInfoLog;
		return;
	}

	// If something went wrong whil compiling the shaders, we'll use this function to find the error
	void PrintShaderCompilationErrorInfo(int32_t shaderId)
	{
		std::cout << "=======================================\n";
		std::cout << "Shader compilation failed : " << std::endl;

		// Find length of shader info log
		int maxLength;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

		// Get shader info log
		char* shaderInfoLog = new char[maxLength];
		glGetShaderInfoLog(shaderId, maxLength, &maxLength, shaderInfoLog);

		// Print shader info log
		std::cout << "\tError info : " << shaderInfoLog << std::endl;

		std::cout << "=======================================\n\n";
		delete []shaderInfoLog;
	}

	GLuint shaderProgram;
	GLuint vertexShader, fragmentShader;

};