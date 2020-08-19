#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{

public:
	unsigned int shaderProgram;

	// constructor will compile and link shaders
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShaderPath = nullptr)
	{
		// retrieve vertex shader and fragment shader source code file
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;

		std::ifstream vertexShaderFile;
		std::ifstream fragmentShaderFile;
		std::ifstream geometryShaderFile;

		// throw exceptions if file reading fails
		vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		geometryShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// open shader files
			vertexShaderFile.open(vertexShaderPath);
			fragmentShaderFile.open(fragmentShaderPath);

			std::stringstream vertexShaderStream;
			std::stringstream fragmentShaderStream;

			// read file
			vertexShaderStream << vertexShaderFile.rdbuf();
			fragmentShaderStream << fragmentShaderFile.rdbuf();

			// close file
			vertexShaderFile.close();
			fragmentShaderFile.close();

			// convert stream to string
			vertexCode = vertexShaderStream.str();
			fragmentCode = fragmentShaderStream.str();

			if (geometryShaderPath != nullptr)
			{
				geometryShaderFile.open(geometryShaderPath);
				std::stringstream geometryShaderStream;
				geometryShaderStream << geometryShaderFile.rdbuf();
				geometryShaderFile.close();
				geometryCode = geometryShaderStream.str();
			}
		}
		catch (std::ifstream::failure& e)
		{
			std::cout << "ERROR::SHADER::FILE_READING_FAILED" << std::endl;
		}

		const char* vertexShaderCode = vertexCode.c_str();
		const char* fragmentShaderCode = fragmentCode.c_str();
		
		// shaders
		unsigned int vertexShader;
		unsigned int fragmentShader;

		// compile vertex shader
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
		glCompileShader(vertexShader);

		// check for vertex shader compile errors
		checkErrors(vertexShader, "VERTEX");
		

		// compile fragment shader
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
		glCompileShader(fragmentShader);

		// check for fragment shader compile errors
		checkErrors(fragmentShader, "FRAGMENT");

		unsigned int geometry;
		if (geometryShaderPath != nullptr)
		{
			const char* geometryShaderCode = geometryCode.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &geometryShaderCode, NULL);
			glCompileShader(geometry);
			checkErrors(geometry,"GEOMETRY");
		}


		// link shaders
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		if (geometryShaderPath != nullptr)
			glAttachShader(shaderProgram, geometry);
		glLinkProgram(shaderProgram);

		// check for linking errors
		checkErrors(shaderProgram, "PROGRAM");

		// delete shaders
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		if (geometryShaderPath != nullptr)
			glDeleteShader(geometry);
	}

	// activate shader
	void useProgram()
	{
		glUseProgram(shaderProgram);
	}

	int getShaderProgramID() {
		return shaderProgram;
	}

	void setBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int)value);
	}

	void setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
	}

	void setFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
	}

	void setVec2(const std::string &name, const glm::vec2 &value) const
	{
		glUniform2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
	}

	void setVec2(const std::string &name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(shaderProgram, name.c_str()), x, y);
	}

	void setVec3(const std::string &name, const glm::vec3 &value) const
	{
		glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
	}

	void setVec3(const std::string &name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z);
	}

	void setVec4(const std::string &name, const glm::vec4 &value) const
	{
		glUniform4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
	}

	void setVec4(const std::string &name, float x, float y, float z, float w) 
	{
		glUniform4f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z, w);
	}

	void setMat2(const std::string& name, const glm::mat2& mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void setMat3(const std::string& name, const glm::mat3& mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void setMat4(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

private:
	// check for shader compilation or linking errors
	void checkErrors(GLuint shaderProgram, std::string errorType)
	{
		GLint success;
		GLchar infoLog[1024];
		if (errorType != "PROGRAM")
		{
			glGetShaderiv(shaderProgram, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shaderProgram, 1024, NULL, infoLog);
				std::cerr << "ERROR::SHADER::" << errorType << "::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shaderProgram, 1024, NULL, infoLog);
				std::cerr << "ERROR::SHADER::" << errorType << "::LINKING_FAILED\n" << infoLog << std::endl;
			}
		}
	}
};


#endif