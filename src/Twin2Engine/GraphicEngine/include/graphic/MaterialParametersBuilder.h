#ifndef _MATERIAL_PARAMETERS_BUILDER_H_
#define _MATERIAL_PARAMETERS_BUILDER_H_

#include <core/YamlConverters.h>

#include <graphic/MaterialParameters.h>

namespace Twin2Engine::GraphicEngine
{
	class MaterialParametersBuilder
	{
		STD140Struct _parameters;

		std::map<size_t, char> _textureMappings;
		std::vector<GLuint> _textures;

		static std::hash<std::string> hasher;
	public:

		template<class T>
		typename std::enable_if_t<is_in_v<T, int, unsigned int, float, double, bool,
										glm::vec2, glm::vec3, glm::vec4,
										glm::ivec2, glm::ivec3, glm::ivec4>>
		Add(const std::string& variableName, const T& value) {
			_parameters.Add(variableName, value);
		}

		void AddTexture2D(const std::string& textureName, unsigned int textureId);

		MaterialParameters* Build();

		void Clear();
	};
}

#endif