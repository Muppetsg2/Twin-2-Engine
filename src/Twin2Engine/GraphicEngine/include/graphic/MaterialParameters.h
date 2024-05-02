#ifndef _MATERIAL_PARAMETERS_H_
#define _MATERIAL_PARAMETERS_H_

#include <core/YamlConverters.h>

namespace Twin2Engine
{
	namespace Manager {
		class MaterialsManager;
	}

	namespace GraphicEngine {
		class MaterialParametersBuilder;

		class MaterialParameters
		{
			friend class Manager::MaterialsManager;
			friend class MaterialParametersBuilder;

			static std::hash<std::string> hasher;

			GLuint _materialParametersDataUBO;

			std::vector<char> _materialData;
			std::map<size_t, unsigned int> _variablesValuesOffsets;

			std::map<size_t, char> _textureMappings;
			std::vector<GLuint> _textures;


			MaterialParameters();
			MaterialParameters(const char* data, size_t size, const std::map<size_t, unsigned int>& variablesValuesOffsets, const std::map<size_t, char>& textureMappings, const std::vector<GLuint>& textures);

			//template<class T>
			//typename std::enable_if<
			//	std::is_same<T, int>::value ||
			//	std::is_same<T, unsigned int>::value ||
			//	std::is_same<T, float>::value ||
			//	std::is_same<T, double>::value ||
			//	std::is_same<T, glm::vec2>::value ||
			//	std::is_same<T, glm::vec3>::value ||
			//	std::is_same<T, glm::vec4>::value ||
			//	std::is_same<T, glm::ivec2>::value ||
			//	std::is_same<T, glm::ivec3>::value ||
			//	std::is_same<T, glm::ivec4>::value, void>::type
			//Add(const std::string& variableName, const T& value);
			//template<class T>
			//typename std::enable_if<std::is_same<T, bool>::value, void>::type
			//	Add(const std::string& variableName, const T& value);
			//
			//void AddTexture2D(const std::string& textureName, unsigned int textureId);
			//
			//void AlignData();

		public:
			MaterialParameters(const std::vector<std::string>& variableNames, const std::vector<unsigned int>& parametersSizes, const std::vector<std::string>& textureParametersNames);

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
				std::is_same<T, glm::ivec4>::value, bool>::type
			Set(const std::string& variableName, const T& value);
			template<class T>
			typename std::enable_if<std::is_same<T, bool>::value, bool>::type
				Set(const std::string& variableName, const T& value);


			void SetTexture2D(const std::string& textureName, unsigned int textureId);
			void UploadTextures2D(unsigned int programId, int& beginLocation, int& textureBinded);

			GLuint GetDataUBO() const;
			//std::vector<char> GetData() const;
			const char* GetData() const;
			size_t GetSize() const;
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
			std::is_same<T, glm::ivec4>::value, bool>::type
		MaterialParameters::Set(const std::string& variableName, const T& value)
		{
			size_t hashed = hasher(variableName);

			if (_variablesValuesOffsets.contains(hashed))
			{
				memcpy(_materialData.data() + _variablesValuesOffsets[hashed], &value, sizeof(T));
				return true;
			}
			return false;
		}
		template<class T>
		typename std::enable_if<std::is_same<T, bool>::value, bool>::type
			MaterialParameters::Set(const std::string& variableName, const T& value)
		{
			size_t hashed = hasher(variableName);

			if (_variablesValuesOffsets.contains(hashed))
			{
				int temp = value;
				memcpy(_materialData.data() + _variablesValuesOffsets[hashed], &temp, sizeof(temp));
				return true;
			}
			return false;
		}

		/*template<class T>
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
		MaterialParameters::Add(const std::string& variableName, const T& value)
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
			MaterialParameters::Add(const std::string& variableName, const T& value)
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
		}*/
	}
}

#endif