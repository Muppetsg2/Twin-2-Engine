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

#include <Shader.h>

//#define ENTIRE_SHADER_PROGRAM_PRECOMPILATION

namespace Twin2Engine::GraphicEngine
{
	// Define the file extension for shader program files
	constexpr char SHADER_PROGRAM_EXTENSION[] = ".shpr";
	// Define the file extension for compiled shader binary files
	constexpr char SHADER_BINARY_EXTENSION[] = ".shdr";

	constexpr char SHADERS_ORIGIN_DIRETORY[] = "ShadersOrigin";

	/// <summary>
	/// Shader Manager obs³uguje:
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

		static const std::unordered_map<size_t, int> shaderTypeMapping;

		static std::hash<std::string> stringHash;
		static std::list<ShaderProgramData*> loadedShaders;

		//Runtime methods
		static unsigned int LoadShaderProgram(const std::string& shaderName);
		static void UnloadShaderProgram(int shaderProgramID);
		static void IncrementUseNumber(int shaderProgramID);

		//Precompilation methods
		static std::string LoadShaderSource(const std::string& filePath);
		static GLuint CompileShader(GLenum type, const std::string& source);

		static std::vector<char> LoadBinarySource(const std::string& filePath);
		//static GLuint CompileShaderSPIRV(GLenum type, const std::vector<char>& source);
		static GLuint CompileShaderSPIRV(GLenum type, const string& filePath);
		//static std::vector<unsigned int> LoadBinarySource(const std::string& filePath);
		//static GLuint CompileShaderSPIRV(GLenum type, const std::vector<unsigned int>& source);

		static inline bool CheckShaderCompilationSuccess(GLuint shaderId);
		static void CheckProgramLinkingSuccess(GLuint programId);
		//static inline void PrecompileShaders();

		//Dynamic creation
		//static inline GLuint CreateShaderProgramFromFile(const std::string& shaderProgramName, std::string& shaderName);
		static inline Shader* LoadShaderProgramSHPR(const std::string& shaderName);
		static inline GLuint CreateShaderProgramFromFile(const std::string& shaderProgramName);
		//static inline void SaveShaderProgramToFile(GLuint shaderProgramId, const std::string& shaderName);

	public:
		//ShaderManager();
		static void Init();
		static void End();

		static Shader* GetShaderProgram(const std::string& shaderName);
		static Shader* CreateShaderProgram(const std::string& shaderProgramName);
		static Shader* CreateShaderProgram(const std::string& shaderName, const std::string& vertexShader, const std::string& fragmentShader);

	};
}


#endif // !SHADER_MANAGER_H

