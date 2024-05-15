#pragma once

#define PARENS ()
#define NEXT_ELEM ,

#define RESCAN(...) RESCAN1(RESCAN1(RESCAN1(RESCAN1(__VA_ARGS__))))
#define RESCAN1(...) RESCAN2(RESCAN2(RESCAN2(RESCAN2(__VA_ARGS__))))
#define RESCAN2(...) RESCAN3(RESCAN3(RESCAN3(RESCAN3(__VA_ARGS__))))
#define RESCAN3(...) RESCAN4(RESCAN4(RESCAN4(RESCAN4(__VA_ARGS__))))
#define RESCAN4(...) __VA_ARGS__

#define DO_FOR_EACH(func, ...)\
	__VA_OPT__(RESCAN(DO_FOR_EACH_HELPER(func, __VA_ARGS__)))
#define DO_FOR_EACH_HELPER(func, a1, ...)\
	func(a1)\
	__VA_OPT__(DO_FOR_EACH_AGAIN PARENS (func, __VA_ARGS__))
#define DO_FOR_EACH_AGAIN() DO_FOR_EACH_HELPER

#define LIST_DO_FOR_EACH(func, ...)\
	__VA_OPT__(RESCAN(LIST_DO_FOR_EACH_HELPER(func, __VA_ARGS__)))
#define LIST_DO_FOR_EACH_HELPER(func, a1, ...)\
	func(a1)\
	__VA_OPT__(NEXT_ELEM LIST_DO_FOR_EACH_AGAIN PARENS (func, __VA_ARGS__))
#define LIST_DO_FOR_EACH_AGAIN() LIST_DO_FOR_EACH_HELPER

#define DO_FOR_EACH_PAIR(func, a1, ...)\
	__VA_OPT__(RESCAN(DO_FOR_EACH_PAIR_HELPER(func, a1, __VA_ARGS__)))
#define DO_FOR_EACH_PAIR_HELPER(func, a1, a2, ...)\
	func(a1, a2)\
	__VA_OPT__(DO_FOR_EACH_PAIR_AGAIN PARENS (func, __VA_ARGS__))
#define DO_FOR_EACH_PAIR_AGAIN() DO_FOR_EACH_PAIR_HELPER

#define LIST_DO_FOR_EACH_PAIR(func, a1, ...)\
	__VA_OPT__(RESCAN(LIST_DO_FOR_EACH_PAIR_HELPER(func, a1, __VA_ARGS__)))
#define LIST_DO_FOR_EACH_PAIR_HELPER(func, a1, a2, ...)\
	func(a1, a2)\
	__VA_OPT__(NEXT_ELEM LIST_DO_FOR_EACH_PAIR_AGAIN PARENS (func, __VA_ARGS__))
#define LIST_DO_FOR_EACH_PAIR_AGAIN() LIST_DO_FOR_EACH_PAIR_HELPER

// STANDARD ENUMS
#define ENUM_ELEMENT(name) name
#define ENUM_CASE(name) case name: return #name;
#define ENUM(name, ...)\
	enum name { LIST_DO_FOR_EACH(ENUM_ELEMENT, __VA_ARGS__) };\
	static std::string to_string(name value) {\
		using enum name;\
		switch(value) {\
		DO_FOR_EACH(ENUM_CASE, __VA_ARGS__)\
		default:\
			return "UNKONWN";\
		}\
	}

#define ENUM_CLASS(name, ...)\
	enum class name { LIST_DO_FOR_EACH(ENUM_ELEMENT, __VA_ARGS__) };\
	static std::string to_string(name value) {\
		using enum name;\
		switch(value) {\
		DO_FOR_EACH(ENUM_CASE, __VA_ARGS__)\
		default:\
			return "UNKNOWN";\
		}\
	}

// ENUMS WITH VALUES
#define ENUM_ELEMENT_VALUE(name, value) name = value
#define ENUM_CASE_VALUE(name, value) case name: return #name;
#define ENUM_VALUE(name, ...)\
	enum name { LIST_DO_FOR_EACH_PAIR(ENUM_ELEMENT_VALUE, __VA_ARGS__) };\
	static std::string to_string(name value) {\
		using enum name;\
		switch(value) {\
		DO_FOR_EACH_PAIR(ENUM_CASE_VALUE, __VA_ARGS__)\
		default:\
			return "UNKONWN";\
		}\
	}

#define ENUM_CLASS_VALUE(name, ...)\
	enum class name { LIST_DO_FOR_EACH_PAIR(ENUM_ELEMENT_VALUE, __VA_ARGS__) };\
	static std::string to_string(name value) {\
		using enum name;\
		switch(value) {\
		DO_FOR_EACH_PAIR(ENUM_CASE_VALUE, __VA_ARGS__)\
		default:\
			return "UNKONWN";\
		}\
	}