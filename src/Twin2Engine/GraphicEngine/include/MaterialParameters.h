#ifndef _MATERIAL_PARAMETERS_H_
#define _MATERIAL_PARAMETERS_H_

#include <string>
#include <unordered_map>
#include <map>
#include <vector>

namespace Twin2Engine::GraphicEngine
{
	class MaterialsManager;

	class MaterialParameters
	{
		friend MaterialsManager;

		std::map<size_t, std::vector<char>> _variablesValuesMappings;
		static std::hash<std::string> hasher;

		MaterialParameters();
		template<class T>
		void Add(const std::string& variableName, T value);
		void Add(const std::string& variableName, size_t size, void* value);

	public:
		MaterialParameters(const std::vector<std::string>& variableNames);

		template<class T>
		bool Set(const std::string& variableName, T value);

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
		std::vector<char> result(ptr, ptr + sizeof(T));
		_variablesValuesMappings[hashed] = result;
	}

}

#endif