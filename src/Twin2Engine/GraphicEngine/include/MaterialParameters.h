#ifndef _MATERIAL_PARAMETERS_H_
#define _MATERIAL_PARAMETERS_H_

#include <string>
#include <unordered_map>
#include <map>
#include <vector>

#include <spdlog/spdlog.h>

namespace Twin2Engine::GraphicEngine
{
	class MaterialsManager;

	class MaterialParameters
	{
		friend MaterialsManager;

		static std::hash<std::string> hasher;
		std::map<size_t, std::vector<char>> _variablesValuesMappings;

		MaterialParameters();
		template<class T>
		void Add(const std::string& variableName, T value);
		template<class T>
		void Add(const std::string& variableName, size_t size, T value);

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
		std::vector<char> result(ptr, ptr + sizeof(value));
		_variablesValuesMappings[hashed] = result;
	}
	template<class T>
	void MaterialParameters::Add(const std::string& variableName, size_t size, T value)
	{
		size_t hashed = hasher(variableName);

		SPDLOG_INFO("Variable name: {}", variableName);
		SPDLOG_INFO("Variable hash: {}", hashed);

		const char* ptr = reinterpret_cast<const char*>(value);
		std::vector<char> result(ptr, ptr + size);
		_variablesValuesMappings[hashed] = result;
	}

}

#endif