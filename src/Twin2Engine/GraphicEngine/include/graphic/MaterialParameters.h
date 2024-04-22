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
			friend class Manager::MaterialsManager;

			std::vector<char> _materialData;
			std::map<size_t, unsigned int> _variablesValuesOffsets;

			std::map<size_t, char> _textureMappings;
			std::vector<GLuint> _textures;

			static std::hash<std::string> hasher;

			MaterialParameters();
			template<class T>
			typename std::enable_if_t<
				std::is_same_v<T, int> ||
				std::is_same_v<T, unsigned int> ||
				std::is_same_v<T, float> ||
				std::is_same_v<T, double> ||
				std::is_same_v<T, glm::vec2> ||
				std::is_same_v<T, glm::vec3> ||
				std::is_same_v<T, glm::vec4> ||
				std::is_same_v<T, glm::ivec2> ||
				std::is_same_v<T, glm::ivec3> ||
				std::is_same_v<T, glm::ivec4>, void>
			Add(const std::string& variableName, const T& value);
			template<class T>
			typename std::enable_if_t<std::is_same_v<T, bool>, void>
				Add(const std::string& variableName, const T& value);

			void AddTexture2D(const std::string& textureName, unsigned int textureId);

			void AlignData();

		public:
			MaterialParameters(const std::vector<std::string>& variableNames, const std::vector<unsigned int>& parametersSizes, const std::vector<std::string>& textureParametersNames);

			template<class T>
			typename std::enable_if_t<
				std::is_same_v<T, int> ||
				std::is_same_v<T, unsigned int> ||
				std::is_same_v<T, float> ||
				std::is_same_v<T, double> ||
				std::is_same_v<T, glm::vec2> ||
				std::is_same_v<T, glm::vec3> ||
				std::is_same_v<T, glm::vec4> ||
				std::is_same_v<T, glm::ivec2> ||
				std::is_same_v<T, glm::ivec3> ||
				std::is_same_v<T, glm::ivec4>, bool>
			Set(const std::string& variableName, const T& value);
			template<class T>
			typename std::enable_if_t<std::is_same_v<T, bool>, bool>
				Set(const std::string& variableName, const T& value);


			template<class T>
			typename std::enable_if_t<
				std::is_same_v<T, int> ||
				std::is_same_v<T, unsigned int> ||
				std::is_same_v<T, float> ||
				std::is_same_v<T, double> ||
				std::is_same_v<T, glm::vec2> ||
				std::is_same_v<T, glm::vec3> ||
				std::is_same_v<T, glm::vec4> ||
				std::is_same_v<T, glm::ivec2> ||
				std::is_same_v<T, glm::ivec3> ||
				std::is_same_v<T, glm::ivec4>, T>
				Get(const std::string& variableName);
			template<class T>
			typename std::enable_if_t<std::is_same_v<T, bool>, T>
				Get(const std::string& variableName);


			void SetTexture2D(const std::string& textureName, unsigned int textureId);
			void UploadTextures2D(unsigned int programId, int& beginLocation, int& textureBinded);

			std::vector<char> GetData() const;
		};

		template<class T>
		typename std::enable_if_t<
			std::is_same_v<T, int> ||
			std::is_same_v<T, unsigned int> ||
			std::is_same_v<T, float> ||
			std::is_same_v<T, double> ||
			std::is_same_v<T, glm::vec2> ||
			std::is_same_v<T, glm::vec3> ||
			std::is_same_v<T, glm::vec4> ||
			std::is_same_v<T, glm::ivec2> ||
			std::is_same_v<T, glm::ivec3> ||
			std::is_same_v<T, glm::ivec4>, bool>
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
		typename std::enable_if_t<std::is_same_v<T, bool>, bool>
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

		template<class T>
		typename std::enable_if_t<
			std::is_same_v<T, int> ||
			std::is_same_v<T, unsigned int> ||
			std::is_same_v<T, float> ||
			std::is_same_v<T, double> ||
			std::is_same_v<T, glm::vec2> ||
			std::is_same_v<T, glm::vec3> ||
			std::is_same_v<T, glm::vec4> ||
			std::is_same_v<T, glm::ivec2> ||
			std::is_same_v<T, glm::ivec3> ||
			std::is_same_v<T, glm::ivec4>, T>
		MaterialParameters::Get(const std::string& variableName) {
			size_t hashed = hasher(variableName);

			if (_variablesValuesOffsets.contains(hashed))
			{
				char* valuePtr = new char[sizeof(T)];
				memcpy(valuePtr, _materialData.data() + _variablesValuesOffsets[hashed], sizeof(T));
				T* value = reinterpret_cast<T*>(valuePtr);
				return *value;
			}
			return T();
		}
		template<class T>
		typename std::enable_if_t<std::is_same_v<T, bool>, T>
			MaterialParameters::Get(const std::string& variableName) {
			size_t hashed = hasher(variableName);

			if (_variablesValuesOffsets.contains(hashed))
			{
				char* valuePtr = new char[sizeof(int)];
				memcpy(valuePtr, _materialData.data() + _variablesValuesOffsets[hashed], sizeof(int));
				int* value = reinterpret_cast<int*>(valuePtr);
				return (bool)*value;
			}
			return T();
		}

		template<class T>
		typename std::enable_if_t<
			std::is_same_v<T, int> ||
			std::is_same_v<T, unsigned int> ||
			std::is_same_v<T, float> ||
			std::is_same_v<T, double> ||
			std::is_same_v<T, glm::vec2> ||
			std::is_same_v<T, glm::vec3> ||
			std::is_same_v<T, glm::vec4> ||
			std::is_same_v<T, glm::ivec2> ||
			std::is_same_v<T, glm::ivec3> ||
			std::is_same_v<T, glm::ivec4>, void>
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
		typename std::enable_if_t<std::is_same_v<T, bool>, void>
			MaterialParameters::Add(const std::string& variableName, const T& value)
		{
			size_t hashed = hasher(variableName);

			unsigned int offset = _materialData.size();
			unsigned int left = 16u - (offset % 16u);
			if (4 > left)
			{
				offset += left;
			}
			_materialData.resize((size_t)offset + 4);
			int temp = value;
			const char* ptr = reinterpret_cast<const char*>(&temp);
			std::memcpy(_materialData.data() + offset, ptr, 4);
			_variablesValuesOffsets[hashed] = offset;
		}
	}
}

#endif