#ifndef _MATERIAL_PARAMETERS_BUILDER_H_
#define _MATERIAL_PARAMETERS_BUILDER_H_

#include <core/YamlConverters.h>

#include <graphic/MaterialParameters.h>

namespace Twin2Engine::GraphicEngine
{
	class MaterialParametersBuilder
	{

		std::vector<char> _materialData;
		std::map<size_t, unsigned int> _variablesValuesOffsets;

		std::map<size_t, char> _textureMappings;
		std::vector<GLuint> _textures;

		static std::hash<std::string> hasher;

		void AlignData();
	public:

		template<class T>
		typename std::enable_if<
			std::is_same<T, int>::value ||
			std::is_same<T, unsigned int>::value ||
			std::is_same<T, float>::value ||
			std::is_same<T, double>::value ||
			std::is_same<T, glm::vec2>::value ||
			std::is_same<T, glm::vec3>::value ||
			std::is_same<T, glm::vec4>::value ||
			std::is_same<T, glm::ivec2>::value ||
			std::is_same<T, glm::ivec3>::value ||
			std::is_same<T, glm::ivec4>::value, void>::type
			Add(const std::string& variableName, const T& value);
		template<class T>
		typename std::enable_if<std::is_same<T, bool>::value, void>::type
			Add(const std::string& variableName, const T& value);

		void AddTexture2D(const std::string& textureName, unsigned int textureId);


		MaterialParameters* Build();

		void Clear();
	};

	template<class T>
	typename std::enable_if<
		std::is_same<T, int>::value ||
		std::is_same<T, unsigned int>::value ||
		std::is_same<T, float>::value ||
		std::is_same<T, double>::value ||
		std::is_same<T, glm::vec2>::value ||
		std::is_same<T, glm::vec3>::value ||
		std::is_same<T, glm::vec4>::value ||
		std::is_same<T, glm::ivec2>::value ||
		std::is_same<T, glm::ivec3>::value ||
		std::is_same<T, glm::ivec4>::value, void>::type
		MaterialParametersBuilder::Add(const std::string& variableName, const T& value)
	{
		size_t hashed = hasher(variableName);

		unsigned int offset = _materialData.size();
		unsigned int left = 16u - (offset % 16u);
		if (sizeof(T) > left)
		{
			offset += left;
		}
		_materialData.resize(offset + sizeof(T));
		const char* ptr = reinterpret_cast<const char*>(&value);
		std::memcpy(_materialData.data() + offset, ptr, sizeof(T));
		_variablesValuesOffsets[hashed] = offset;
	}

	template<class T>
	typename std::enable_if<std::is_same<T, bool>::value, void>::type
		MaterialParametersBuilder::Add(const std::string& variableName, const T& value)
	{
		size_t hashed = hasher(variableName);

		unsigned int offset = _materialData.size();
		unsigned int left = 16u - (offset % 16u);
		if (4 > left)
		{
			offset += left;
		}
		_materialData.resize(offset + 4);
		int temp = value;
		const char* ptr = reinterpret_cast<const char*>(&temp);
		std::memcpy(_materialData.data() + offset, ptr, 4);
		_variablesValuesOffsets[hashed] = offset;
	}
}

#endif