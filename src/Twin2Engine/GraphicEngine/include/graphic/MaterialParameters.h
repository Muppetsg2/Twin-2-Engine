#ifndef _MATERIAL_PARAMETERS_H_
#define _MATERIAL_PARAMETERS_H_

#include <core/YamlConverters.h>

namespace Twin2Engine
{
	namespace Manager {
		class MaterialsManager;
	}

	namespace GraphicEngine {
		class MaterialParameters
		{
			struct MaterialParametersValue
			{
				unsigned int offset;
				unsigned int size;
			};
			friend class Manager::MaterialsManager;
			std::vector<char> _materialData;
			//std::map<size_t, std::vector<char>> _variablesValuesMappings;
			//std::map<size_t, MaterialParametersValue> _variablesValuesMappings;
			std::map<size_t, unsigned int> _variablesValuesOffsets;
			std::map<size_t, char> _textureMappings;
			std::vector<GLuint> _textures;
			//std::vector<GLuint> _samplers;

			static std::hash<std::string> hasher;

			MaterialParameters();
			template<class T>
			typename std::enable_if<
				std::is_same<T, int>::value ||
				std::is_same<T, unsigned int>::value ||
				std::is_same<T, float>::value ||
				std::is_same<T, double>::value ||
				std::is_same<T, bool>::value ||
				std::is_same<T, glm::vec2>::value ||
				std::is_same<T, glm::vec3>::value ||
				std::is_same<T, glm::vec4>::value ||
				std::is_same<T, glm::ivec2>::value ||
				std::is_same<T, glm::ivec3>::value ||
				std::is_same<T, glm::ivec4>::value, void>::type
			Add(const std::string& variableName, const T& value);
			//template<class T>
			//typename std::enable_if<
			//	std::is_same<T, int>::value ||
			//	std::is_same<T, unsigned int>::value ||
			//	std::is_same<T, float>::value ||
			//	std::is_same<T, double>::value ||
			//	std::is_same<T, bool>::value ||
			//	std::is_same<T, glm::vec2>::value ||
			//	std::is_same<T, glm::vec3>::value ||
			//	std::is_same<T, glm::vec4>::value ||
			//	std::is_same<T, glm::ivec2>::value ||
			//	std::is_same<T, glm::ivec3>::value ||
			//	std::is_same<T, glm::ivec4>::value, void>::type
			//	Add(const std::string& variableName, void* valuePtr);
			//void Add(const std::string& variableName, size_t size, void* value);

			void AlignData();

			//void AddInt(const std::string& variableName, int value);
			//void AddUInt(const std::string& variableName, unsigned int value);
			//void AddFloat(const std::string& variableName, unsigned int value);
			//void AddDouble(const std::string& variableName, unsigned int value);
			//void AddVector2(const std::string& variableName, unsigned int value);
			//void AddVector3(const std::string& variableName, unsigned int value);
			//void AddVector4(const std::string& variableName, unsigned int value);
			//void AddIVector2(const std::string& variableName, unsigned int value);
			//void AddIVector3(const std::string& variableName, unsigned int value);
			//void AddIVector4(const std::string& variableName, unsigned int value);

			void AddTexture2D(const std::string& textureName, unsigned int textureId);
		public:
			MaterialParameters(const std::vector<std::string>& variableNames, const std::vector<std::string>& textureParametersNames);

			template<class T>
			typename std::enable_if<
				std::is_same<T, int>::value ||
				std::is_same<T, unsigned int>::value ||
				std::is_same<T, float>::value ||
				std::is_same<T, double>::value ||
				std::is_same<T, bool>::value ||
				std::is_same<T, glm::vec2>::value ||
				std::is_same<T, glm::vec3>::value ||
				std::is_same<T, glm::vec4>::value ||
				std::is_same<T, glm::ivec2>::value ||
				std::is_same<T, glm::ivec3>::value ||
				std::is_same<T, glm::ivec4>::value, bool>::type
			Set(const std::string& variableName, const T& value);


			void SetTexture2D(const std::string& textureName, unsigned int textureId);
			void UploadTextures2D(unsigned int programId, int& beginLocation, int& textureBinded);

			std::vector<char> GetData() const;
		};

		template<class T>
		typename std::enable_if<std::is_same<T, int>::value ||
			std::is_same<T, unsigned int>::value ||
			std::is_same<T, float>::value ||
			std::is_same<T, double>::value ||
			std::is_same<T, bool>::value ||
			std::is_same<T, glm::vec2>::value ||
			std::is_same<T, glm::vec3>::value ||
			std::is_same<T, glm::vec4>::value ||
			std::is_same<T, glm::ivec2>::value ||
			std::is_same<T, glm::ivec3>::value ||
			std::is_same<T, glm::ivec4>::value, bool>::type
		MaterialParameters::Set(const std::string& variableName, const T& value)
		{
			size_t hashed = hasher(variableName);

			//if (_variablesValuesMappings.contains(hashed))
			if (_variablesValuesOffsets.contains(hashed))
			{
				//memcpy(_materialData.data() + _variablesValuesMappings[hashed].offset, &value, sizeof(T));
				memcpy(_materialData.data() + _variablesValuesOffsets[hashed], &value, sizeof(T));
				return true;
			}
			return false;
		}

		template<class T>
		typename std::enable_if<
			std::is_same<T, int>::value ||
			std::is_same<T, unsigned int>::value ||
			std::is_same<T, float>::value ||
			std::is_same<T, double>::value ||
			std::is_same<T, bool>::value ||
			std::is_same<T, glm::vec2>::value ||
			std::is_same<T, glm::vec3>::value ||
			std::is_same<T, glm::vec4>::value ||
			std::is_same<T, glm::ivec2>::value ||
			std::is_same<T, glm::ivec3>::value ||
			std::is_same<T, glm::ivec4>::value, void>::type
		MaterialParameters::Add(const std::string& variableName, const T& value)
		{
			size_t hashed = hasher(variableName);

			unsigned int offset = _materialData.size();
			//unsigned int left = 16u - (_materialData.size() % 16u);
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
		//template<class T>
		//typename std::enable_if<
		//	std::is_same<T, int>::value ||
		//	std::is_same<T, unsigned int>::value ||
		//	std::is_same<T, float>::value ||
		//	std::is_same<T, double>::value ||
		//	std::is_same<T, bool>::value ||
		//	std::is_same<T, glm::vec2>::value ||
		//	std::is_same<T, glm::vec3>::value ||
		//	std::is_same<T, glm::vec4>::value ||
		//	std::is_same<T, glm::ivec2>::value ||
		//	std::is_same<T, glm::ivec3>::value ||
		//	std::is_same<T, glm::ivec4>::value, void>::type
		//	MaterialParameters::Add(const std::string& variableName, void* valuePtr)
		//{
		//	size_t hashed = hasher(variableName);
		//
		//	//unsigned int left = 16u - (_materialData.size() % 16u);
		//	unsigned int offset = _materialData.size();
		//	if (sizeof(T) > left)
		//	{
		//		offset += 16u - (offset % 16u);
		//	}
		//	_materialData.resize(offset + sizeof(T));
		//	const char* ptr = reinterpret_cast<const char*>(valuePtr);
		//	std::memcpy(_materialData.data() + offset, ptr, sizeof(T));
		//	_variablesValuesOffsets[hashed] = offset;
		//}
	}
}

#endif