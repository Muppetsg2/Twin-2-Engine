#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <string>
#include <unordered_map>

using std::string;

#include <graphic/Shader.h>

namespace Twin2Engine::Manager
{
	// Define the file extension for shader program files
	constexpr char SHADER_PROGRAM_EXTENSION[] = ".shpr";
	// Define the file extension for compiled shader binary files
	constexpr char SHADER_BINARY_EXTENSION[] = ".shdr";

	constexpr char SHADERS_ORIGIN_DIRETORY[] = "ShadersOrigin";

	/// <summary>
	/// Shader Manager obs�uguje:
	/// * vertex shader
	/// * geometry shader
	/// * fragment shader
	/// </summary>
	class ShaderManager
	{
		friend class GraphicEngine::Shader;

		struct ShaderProgramData
		{
			size_t shaderPathHash;
			int shaderProgramId;
			int useNumber;
			GraphicEngine::Shader* shader;
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
		static inline bool CheckShaderCompilationSuccess(GLuint shaderId);
		static void CheckProgramLinkingSuccess(GLuint programId);
		static inline void PrecompileShaders();

		//Dynamic creation
		static inline GLuint CreateShaderProgramFromFile(const std::string& shaderProgramName, std::string& shaderName);
		static inline void SaveShaderProgramToFile(GLuint shaderProgramId, const std::string& shaderName);

	public:
		//ShaderManager();
		static void Init();
		static void End();

		static GraphicEngine::Shader* GetShaderProgram(const std::string& shaderName);
		static GraphicEngine::Shader* CreateShaderProgram(const std::string& shaderProgramName);
		static GraphicEngine::Shader* CreateShaderProgram(const std::string& shaderName, const std::string& vertexShader, const std::string& fragmentShader);

	};
}


#endif // !SHADER_MANAGER_H
