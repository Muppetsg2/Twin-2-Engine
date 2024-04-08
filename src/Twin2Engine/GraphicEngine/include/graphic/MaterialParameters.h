#ifndef _MATERIAL_PARAMETERS_H_
#define _MATERIAL_PARAMETERS_H_

namespace Twin2Engine
{
	namespace Manager {
		class MaterialsManager;
	}

	namespace GraphicEngine {
		class MaterialParameters
		{
			friend class Manager::MaterialsManager;

			std::map<size_t, std::vector<char>> _variablesValuesMappings;
			std::map<size_t, char> _textureMappings;
			std::vector<GLuint> _textures;
			std::vector<GLuint> _samplers;

			static std::hash<std::string> hasher;

			MaterialParameters();
			template<class T>
			void Add(const std::string& variableName, T value);
			void Add(const std::string& variableName, size_t size, void* value);

			void AddTexture2D(const std::string& textureName, unsigned int textureId);
		public:
			MaterialParameters(const std::vector<std::string>& variableNames);

			template<class T>
			bool Set(const std::string& variableName, T value);

			void SetTexture2D(const std::string& textureName, unsigned int textureId);
			void UploadTextures2D(unsigned int programId, int& beginLocation, int& textureBinded);

			std::vector<char> GetData() const;
		};

		template<class T>
		bool MaterialParameters::Set(const std::string& variableName, T value)
		{
			size_t hashed = hasher(variableName);

			if (_variablesValuesMappings.contains(hashed))
			{
				const char* ptr = reinterpret_cast<const char*>(&value);
				std::vector<char> result(ptr, ptr + sizeof(value));
				_variablesValuesMappings[hashed] = result;
				return true;
			}
			return false;
		}

		template<class T>
		void MaterialParameters::Add(const std::string& variableName, T value)
		{
			size_t hashed = hasher(variableName);

			const char* ptr = reinterpret_cast<const char*>(&value);
			std::vector<char> result(ptr, ptr + sizeof(value));
			_variablesValuesMappings[hashed] = result;
		}
	}
}

#endif