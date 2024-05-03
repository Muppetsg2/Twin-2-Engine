#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <graphic/Shader.h>

#define ENTIRE_SHADER_PROGRAM_PRECOMPILATION false
#define SPIRV_COMPILATION false
#define NORMAL_SHADERS_CREATION true

namespace Twin2Engine::Manager
{
	// Define the file extension for shader program files
	constexpr char SHADER_PROGRAM_EXTENSION[] = ".shpr";
	// Define the file extension for compiled shader binary files
	constexpr char SHADER_BINARY_EXTENSION[] = ".shdr";

	constexpr char SHADERS_ORIGIN_DIRETORY[] = "res";

	/// <summary>
	/// Shader Manager obs³uguje:
	/// * vertex shader
	/// * geometry shader
	/// * fragment shader
	/// </summary>
	class ShaderManager
	{
		friend class Graphic::Shader;

		struct ShaderProgramData
		{
			size_t shaderPathHash;
			unsigned int shaderProgramId;
			int useNumber;
			Graphic::Shader* shader;
			bool operator<(const ShaderProgramData& other) const {
				return shaderPathHash < other.shaderPathHash;
			}
		};

		static GLenum binaryFormat;

		static const std::unordered_map<size_t, int> shaderTypeMapping;

		static std::hash<std::string> stringHash;
		static std::list<ShaderProgramData> loadedShaders;

		//Runtime methods
		static unsigned int LoadShaderProgram(const std::string& shaderName);
		static void UnloadShaderProgram(int shaderProgramID);
		static void IncrementUseNumber(int shaderProgramID);

		//Precompilation methods
		static std::string LoadShaderSource(const std::string& filePath);
		static GLuint CompileShader(GLenum type, const std::string& source);

		static std::vector<char> LoadBinarySource(const std::string& filePath);
		static GLuint CompileShaderSPIRV(GLenum type, const std::string& filePath);

		static inline bool CheckShaderCompilationSuccess(GLuint shaderId);
		static inline void CheckProgramLinkingSuccess(GLuint programId);

		//Dynamic creation
		static inline Graphic::Shader* LoadShaderProgramSHPR(const std::string& shaderName);
		static inline GLuint CreateShaderProgramFromFile(const std::string& shaderProgramName);

	public:
		static Graphic::Shader* DepthShader;

		static void Init();
		static void UnloadAll();

		static Graphic::Shader* GetShaderProgram(const std::string& shaderName);
		static Graphic::Shader* CreateShaderProgram(const std::string& shaderName, const std::string& vertexShader, const std::string& fragmentShader);

		void UpdateDirShadowMapsTab();
	};
}


#endif // !SHADER_MANAGER_H

