#pragma once

#include <functional>

namespace Twin2Engine {
	namespace UI {
		template<typename Ty> using is_void = std::is_same<Ty, void>;
		template<typename Ty, typename T, typename F> using if_void = std::conditional<is_void<Ty>::value, T, F>::type;

		template<typename... Args> using Action = if_void<Args..., std::function<void(void)>, std::function<void(Args...)>>;
		using Method = Action<void>;
		template<typename Ret, typename... Args> using Func = if_void<Args..., std::function<Ret(void)>, if_void<Ret, Action<Args>, std::function<Ret(Args...)>>>;

		template<typename... Args>
		class EventHandler {
		private:
			std::vector<Action<Args>> _actions = std::vector<Action<Args>>();

		public:
			EventHandler() = default;
			virtual ~EventHandler() = default;

			constexpr void AddCallback(const Action<Args>& callback) {
				_actions.push_back(callback);
			}
			void RemoveCallback(const Action<Args>& callback) {
				for (size_t i = 0; i < _actions.size(); ++i) {
					if (_actions[i] == callback) {
						_actions.erase(_actions.begin() + i);
						break;
					}
				}
			}
			constexpr void RemoveAllCallbacks() {
				_actions.clear();
			}


			void Invoke(Args... args) const {
				for (Action<Args>& action : _actions) {
					action(args);
				}
			}

			constexpr void operator+=(const Action<Args>& callback) {
				AddCallback(callback);
			}
			constexpr void operator-=(const Action<Args>& callback) {
				RemoveCallback(callback);
				return *this;
			}
		};
		using MethodEventHandler = EventHandler<void>;
	}
}