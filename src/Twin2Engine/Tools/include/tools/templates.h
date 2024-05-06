#pragma once

namespace Twin2Engine::Tools {
	template<class T, class... Ts> constexpr bool is_type_in_v = (std::is_same_v<T, Ts> || ...);

	template<auto A, auto B> constexpr bool is_eq_v = (A == B);
	template<auto A, auto... Bs> constexpr bool is_in_v = (is_eq_v<A, Bs> || ...);

	template<size_t N, size_t... Ns> constexpr bool is_num_in_v = is_in_v<N, Ns...>;

	template<auto A, auto B> constexpr bool is_gteq = (A >= B);
	template<auto A, auto B> constexpr bool is_lteq = (A <= B);

	template<auto A, auto Min, auto Max> constexpr bool is_in_range_v = (is_gteq<A, Min> && is_lteq<A, Max>);

	template<size_t N, size_t Min, size_t Max> constexpr bool is_num_in_range_v = is_in_range_v<N, Min, Max>;

	template<bool Test, class Ta, class Tb> struct type_test {
		using type = Ta;
	};
	template<class Ta, class Tb> struct type_test<false, Ta, Tb> {
		using type = Tb;
	};
	template<bool Test, class Ta, class Tb> using type_test_t = type_test<Test, Ta, Tb>::type;
}