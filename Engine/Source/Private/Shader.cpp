#include "Shader.h"

#include <filesystem>

Shader::Shader()
{
	ID = -1;
}

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* tessellationControlPath, const char* tessellationEvaluationPath)
{
	Load(vertexPath, fragmentPath, tessellationControlPath, tessellationEvaluationPath);
}

void Shader::Load(const char* vertexPath, const char* fragmentPath, const char* tessellationControlPath, const char* tessellationEvaluationPath)
{
	std::string vertexCode, fragmentCode, tessellationControlCode, tessellationEvaluationCode;
	std::ifstream vShaderFile, fShaderFile, tcShaderFile, teShaderFile;
	
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		if (tessellationControlPath)
		{
			tcShaderFile.open(tessellationControlPath);
			std::stringstream tcShaderStream;
			tcShaderStream << tcShaderFile.rdbuf();
			tcShaderFile.close();
			tessellationControlCode = tcShaderStream.str();
		}
		if (tessellationEvaluationPath)
		{
			teShaderFile.open(tessellationEvaluationCode);
			std::stringstream teShaderStream;
			teShaderStream << teShaderFile.rdbuf();
			teShaderFile.close();
			tessellationEvaluationCode = teShaderStream.str();
		}
	}
	catch (std::ifstream::failure& e)
	{
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ " << "vertexPath: " << vertexPath << ", fragmentPath: " << fragmentPath << "\n" << e.what() << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	const char* tcShaderCode = tessellationControlPath ? tessellationControlCode.c_str() : nullptr;
	const char* teShaderCode = tessellationEvaluationPath ? tessellationEvaluationCode.c_str() : nullptr;

	unsigned int vertex, fragment, tessellationControl, tessellationEvaluation;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	CheckCompileErrors(vertex, "VERTEX");

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	CheckCompileErrors(fragment, "FRAGMENT");

	if (tessellationControlPath)
	{
		tessellationControl = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(tessellationControl, 1, &tcShaderCode, NULL);
		glCompileShader(tessellationControl);
		CheckCompileErrors(tessellationControl, "TESS_CONTROL");
	}

	if (tessellationEvaluationPath)
	{
		tessellationEvaluation = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(tessellationEvaluation, 1, &teShaderCode, NULL);
		glCompileShader(tessellationEvaluation);
		CheckCompileErrors(tessellationEvaluation, "TESS_EVALUATION");
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if (tessellationControlPath)
		glAttachShader(ID, tessellationControl);
	if (tessellationEvaluationPath)
		glAttachShader(ID, tessellationEvaluation);
	glLinkProgram(ID);
	CheckCompileErrors(ID, "PROGRAM");

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (tessellationControlPath)
		glDeleteShader(tessellationControl);
	if (tessellationEvaluationPath)
		glDeleteShader(tessellationEvaluation);
}

Shader::~Shader()
{

}

void Shader::Use() const
{
	glUseProgram(ID);
}

void Shader::SetBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::SetVec2(const std::string& name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::SetVec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::SetVec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::SetVec4(const std::string &name, const glm::vec4 &value) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::SetVec4(const std::string &name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::SetMat2(const std::string &name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::SetMat3(const std::string &name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::SetMat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetIntArray(const std::string& name, const std::vector<int>& value) const
{
	glUniform1iv(glGetUniformLocation(ID, name.c_str()), value.size(), value.data());
}

void Shader::SetFloatArray(const std::string& name, const std::vector<float>& value) const
{
	glUniform1fv(glGetUniformLocation(ID, name.c_str()), value.size(), value.data());
}

void Shader::CheckCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
			std::abort();
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
			std::abort();
		}
	}
}
