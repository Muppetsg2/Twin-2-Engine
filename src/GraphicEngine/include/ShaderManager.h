#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <iostream>
#include <filesystem>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

#include <glad/glad.h>

using std::string;

#include "Shader.h"

namespace GraphicEngine
{
	// Define the file extension for shader program files
	constexpr char SHADER_PROGRAM_EXTENSION[] = ".shpr";
	// Define the file extension for compiled shader binary files
	constexpr char SHADER_BINARY_EXTENSION[] = ".shdr";

	/// <summary>
	/// Shader Manager obs�uguje:
	/// * vertex shader
	/// * geometry shader
	/// * fragment shader
	/// </summary>
	class ShaderManager
	{
		friend class Shader;

		struct ShaderProgramData
		{
			size_t shaderPathHash;
			int shaderProgramId;
			int useNumber;
			Shader* shader;
			bool operator<(const ShaderProgramData& other) const {
				return shaderPathHash < other.shaderPathHash;
			}
		};

		static GLenum binaryFormat;

		static std::hash<std::string> stringHash;
		static std::list<ShaderProgramData*> loadedShaders;

		//Runtime methods
		static unsigned int LoadShaderProgram(const std::string& shaderPath);
		static void UnloadShaderProgram(int shaderProgramID);
		static void IncrementUseNumber(int shaderProgramID);

		//Precompilation methods
		static void PrecompileShaders();
		static std::string LoadShaderSource(const std::string& filePath);
		static GLuint CompileShader(GLenum type, const std::string& source);
		static void CheckShaderCompilationSuccess(GLuint shaderId);
		static void CheckProgramLinkingSuccess(GLuint programId);

	public:
		//ShaderManager();
		static void Init();
		static void End();

		static Shader* GetShaderProgram(const std::string& shaderName);
		static Shader* CreateShaderProgram(const std::string& shaderName, const std::string& vertexShader, const std::string& fragmentShader);

	};
}


#endif // !SHADER_MANAGER_H

