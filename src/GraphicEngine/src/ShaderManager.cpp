#include "ShaderManager.h"

#include <iostream>

void GraphicEngine::ShaderManager::checkShaderCompilationSuccess(GLuint shaderId)
{
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
		std::cout << shaderId << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << "\n"; //Change to logging
	}
}

unsigned int GraphicEngine::ShaderManager::FindShader(const string & shaderSourcePath, int shaderType)
{
	std::vector<ShaderRecord>::iterator begin;
	std::vector<ShaderRecord>::iterator end;

	switch (shaderType)
	{
	case GL_VERTEX_SHADER:
		begin = vertexShaders.begin();
		end = vertexShaders.end();
		break;

	case GL_GEOMETRY_SHADER:
		begin = geometryShaders.begin();
		end = geometryShaders.end();
		break;

	case GL_FRAGMENT_SHADER:
		begin = fragmentShaders.begin();
		end = fragmentShaders.end();
		break;
	}

	std::size_t hashed = Hash(shaderSourcePath);
	std::vector<ShaderRecord>::iterator found = std::find_if(begin, end, [hashed](const ShaderRecord& shaderRecord) { return shaderRecord.shaderHash == hashed; });

	unsigned int foundId = 0;
	if (found != end)
	{
		foundId = found->shaderId;
	}

	return foundId;
}

unsigned int GraphicEngine::ShaderManager::ImportShader(const string& shaderSourcePath, int shaderType)
{
	return 0;
}

unsigned int GraphicEngine::ShaderManager::CreateShader(const string& shaderCode, int shaderType)
{
	unsigned int shaderID = glCreateShader(shaderType);
	const char* const shaderSource = (const char*)shaderCode.c_str();
	glShaderSource(shaderID, 1, &shaderSource, NULL);
	glCompileShader(shaderID);

	checkShaderCompilationSuccess(shaderID);

	return shaderID;
}

unsigned int GraphicEngine::ShaderManager::GetShaderInstance(const string& sourcePath, int shaderType)
{
	unsigned int shaderId = FindShader(sourcePath, shaderType);

	if (shaderId == 0)
	{
		shaderId = ImportShader(sourcePath, shaderType);
	}

	return shaderId;
}

void GraphicEngine::ShaderManager::StartShaderCreation()
{
}

void GraphicEngine::ShaderManager::FinishShaderCreation()
{
	int i = 0;

	for (i = 0; i < vertexShaders.size(); i++)
	{
		glDeleteShader(vertexShaders[i].shaderId);
	}

	for (i = 0; i < geometryShaders.size(); i++)
	{
		glDeleteShader(geometryShaders[i].shaderId);
	}

	for (i = 0; i < fragmentShaders.size(); i++)
	{
		glDeleteShader(fragmentShaders[i].shaderId);
	}
}
