#pragma once

#include <vector>
#include <string>

template <class /* To */, class /* From */>
struct Unique_impl;
template <class ... Ts>
struct Unique_impl<std::tuple<Ts...>, std::tuple<>>
{
	using type = std::tuple<Ts...>;
};
template <class T, class ... Ts, class ... Us>
struct Unique_impl<std::tuple<Us...>, std::tuple<T, Ts...>> :
	std::conditional_t
	< (std::is_same_v<T, Us> || ...)                 // T in internal list?
	, Unique_impl<std::tuple<Us...>, std::tuple<Ts...>>    // Yes: We skip it
	, Unique_impl<std::tuple<Us..., T>, std::tuple<Ts...>> // No:  We add it
	>
{};
template <class... Ts>
using Unique = Unique_impl<std::tuple<>, std::tuple<Ts...>>;
template <class... Ts>
using Unique_t = typename Unique<Ts...>::type;

static_assert(std::is_same_v<Unique_t<int, int, void>, std::tuple<int, void>>);

template<typename T, typename... Ts> struct has_type
	: std::conditional_t<((!std::is_same_v<T, Ts>) && ...),
	std::false_type,
	std::true_type> {};

static_assert(has_type<int, int, void, bool>::value);

template <size_t Length>
struct fixed_string {
	char _chars[Length + 1] = {}; // +1 for null terminator
};

template <size_t N>
fixed_string(const char(&arr)[N]) -> fixed_string<N - 1>;

fixed_string str = fixed_string("Hello");

template<fixed_string>
struct type_name {};

template<>
struct type_name<fixed_string("string")> { using type = const char*; };

template<fixed_string S>
using type_name_t = type_name<S>::type;

/*template<typename... Types>
class TypesMap {
private:
	std::vector<std::string> _names = std::vector<std::string>();

public:
	template<std::enable_if_t<(sizeof...(Types) == 0), bool> = true> 
	TypesMap() {

	}
	template<type_name... Args, std::enable_if_t<(sizeof...(Types) > 0 && sizeof...(Args) == sizeof...(Types)), bool> = true> 
	TypesMap(Args... args) {

	}
	virtual ~TypesMap() = default;

	template<typename T> typename T GetType(const std::string& name) {
		size_t i;
		for (i = 0; i < _names.size(); ++i) {
			if (_names[i] == name) {
				break;
			}
		}
		return std::tuple_element_t<i, std::tuple<Types...>>;
	}
};*/