#ifndef _MATERIAL_PARAMETERS_BUILDER_H_
#define _MATERIAL_PARAMETERS_BUILDER_H_

#include <tools/YamlConverters.h>

#include <graphic/MaterialParameters.h>

namespace Twin2Engine::Graphic
{
	class MaterialParametersBuilder
	{
		Tools::STD140Struct _parameters;

		std::map<size_t, char> _textureMappings;
		std::vector<GLuint> _textures;

		static std::hash<std::string> hasher;
	public:

		template<class T>
		typename std::enable_if_t<Tools::is_type_in_v<T, int, uint32_t, float, double, bool>>
		Add(const std::string& variableName, const T& value) {
			_parameters.Add(variableName, value);
		}

		template<class V, class T = V::value_type, size_t L = V::length()>
		typename std::enable_if_t<std::is_same_v<V, glm::vec<L, T>> 
								&& Tools::is_type_in_v<T, int, uint32_t, float, double, bool>
								&& Tools::is_num_in_range_v<L, 1, 4>>
		Add(const std::string& variableName, const V& value) {
			_parameters.Add(variableName, value);
		}

		void AddTexture2D(const std::string& textureName, unsigned int textureId);

		MaterialParameters* Build();

		void Clear();
	};
}

#endif