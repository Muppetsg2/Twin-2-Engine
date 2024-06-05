#pragma once

#include <tools/YamlConverters.h>
#include <tools/STD140Struct.h>

namespace Twin2Engine
 {
	namespace Manager {
		class MaterialsManager;
	}

	namespace Graphic {
		class MaterialParametersBuilder;

		class MaterialParameters
		{
			friend class Manager::MaterialsManager;
			friend class MaterialParametersBuilder;

			static std::hash<std::string> hasher;

			GLuint _materialParametersDataUBO = 0;

			Tools::STD140Struct _parameters;

#if _DEBUG
			std::map<size_t, std::string> _textureNames;
#endif

			std::map<size_t, char> _textureMappings;
			std::vector<GLuint> _textures;


			MaterialParameters();
			MaterialParameters(const Tools::STD140Struct& parameters, const std::map<size_t, char>& textureMappings, const std::vector<GLuint>& textures);

		public:
			template<class T>
			typename std::enable_if_t<Tools::is_type_in_v<T, bool, int, uint32_t, float, double>, bool>
			Set(const std::string& variableName, const T& value) {
				if (_parameters.Set(variableName, value)) {
					glBindBuffer(GL_UNIFORM_BUFFER, _materialParametersDataUBO);
					glBufferSubData(GL_UNIFORM_BUFFER, 0, _parameters.GetSize(), _parameters.GetData().data());
					glBindBuffer(GL_UNIFORM_BUFFER, NULL);
					return true;
				}
				return false;
			}

			template<class V, class T = V::value_type, size_t L = V::length()>
			typename std::enable_if_t<std::is_same_v<V, glm::vec<L, T>>
									&& Tools::is_type_in_v<T, bool, int, uint32_t, float, double>
									&& Tools::is_num_in_range_v<L, 1, 4>, bool>
			Set(const std::string& variableName, const V& value) {
				if (_parameters.Set(variableName, value)) {
					glBindBuffer(GL_UNIFORM_BUFFER, _materialParametersDataUBO);
					glBufferSubData(GL_UNIFORM_BUFFER, 0, _parameters.GetSize(), _parameters.GetData().data());
					glBindBuffer(GL_UNIFORM_BUFFER, NULL);
					return true;
				}
				return false;
			}


			~MaterialParameters();

			template<class T>
			typename std::enable_if_t<Tools::is_type_in_v<T, bool, int, uint32_t, float, double>, T>
			Get(const std::string& variableName) const {
				return _parameters.Get<T>(variableName);
			}

			template<class V, class T = V::value_type, size_t L = V::length()>
			typename std::enable_if_t<std::is_same_v<V, glm::vec<L, T>>
									&& Tools::is_type_in_v<T, bool, int, uint32_t, float, double>
									&& Tools::is_num_in_range_v<L, 1, 4>, V>
			Get(const std::string& variableName) const {
				return _parameters.Get<V>(variableName);
			}

#if _DEBUG
			YAML::Node Serialize() const;
			void DrawEditor(size_t id);
#endif

			void SetTexture2D(const std::string& textureName, unsigned int textureId);
			void UploadTextures2D(unsigned int programId, int& beginLocation, int& textureBinded);

			GLuint GetDataUBO() const;
			const char* GetData() const;
			size_t GetSize() const;
		};
	}
}