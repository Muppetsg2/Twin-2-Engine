#pragma once

#include <vector>
#include <string>

template<typename... Types>
class TypesMap {
private:
	std::vector<std::string> _names;

public:
	template<std::enable_if_t<sizeof...(Types...) > 0, bool> = true>
	TypesMap(const std::vector<std::string>& names) {

	}
	template<false> TypesMap() = default;
	virtual ~TypesMap() = default;

	template<typename T, typename... R> TypesMap<R...> AddType(const std::string& name) {
		if (_names.find(name) != _names.end()) {
			std::vector<std::string> names = _names;
			name.push_back(name);
			return TypesMap<Types..., T>(names);
		}
		return TypesMap<Types...>();
	}
};