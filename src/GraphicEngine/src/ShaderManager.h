#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <functional>
#include <string>
#include <vector>

#include <glad/glad.h>

using std::string;

namespace GraphicEngine
{
	/// <summary>
	/// Shader Manager obs³uguje:
	/// * vertex shader
	/// * geometry shader
	/// * fragment shader
	/// </summary>
	class ShaderManager
	{
		struct ShaderRecord
		{
			int shaderHash;
			//const char* shaderSourcePath;
			unsigned int shaderId;

		};

		std::hash<string> Hash;

		std::vector<ShaderRecord> vertexShaders;
		std::vector<ShaderRecord> geometryShaders;
		std::vector<ShaderRecord> fragmentShaders;


		void checkShaderCompilationSuccess(GLuint shaderId);
		unsigned int FindShader(const string & shaderSourcePath, int shaderType);
		unsigned int ImportShader(const string & shaderSourcePath, int shaderType);
		unsigned int CreateShader(const string& shaderCode, int shaderType);

	public:
		unsigned int GetShaderInstance(const string& sourcePath, int shaderType);

		void StartShaderCreation();
		void FinishShaderCreation();

	};
}


#endif // !SHADER_MANAGER_H

