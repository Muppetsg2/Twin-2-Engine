#pragma once

/*#define STRING_ENUM_MAP_PAIR(name, x) { name::x, #x }
#define STRINGABLE_ENUM(name, ...)\
		enum class name {__VA_ARGS__};\
		static std::map<name, std::string> name##Strings { MAP_LIST(STRING_ENUM_MAP_PAIR, name, __VA_ARGS__) };\
		std::string to_string(name value) { return name##Strings[value]; }*/

#define ENUM_CASE(name) case name: return #name;
#define ENUM(name, ...)\
	enum name { __VA_ARGS__ };\
	std::string to_string(name value) {\
		using enum name;\
		switch(value) {\
		FOR_EACH(ENUM_CASE, __VA_ARGS__)\
		default:\
			return "UNKONWN";\
		}\
	}\