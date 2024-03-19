#pragma once

#include <functional>

namespace Twin2Engine {
	namespace UI {
		template<typename V, typename T, typename F> using is_void = typename std::conditional<std::is_same<V, void>::value, T, F>;

		template<typename... Args> using Action = typename is_void<Args, std::function<void()>, std::function<void(Args)>>;
		template<typename Ret, typename... Args> using Func = typename is_void<Ret, Action<Args>, typename is_void<Args, std::function<Ret(void)>, std::function<Ret(Args)>>;

		//template<typename... Args>
		class EventHandler {
			std::vector<Action<void>> _onClickActions = std::vector<Action<void>>();
		};
	}
}