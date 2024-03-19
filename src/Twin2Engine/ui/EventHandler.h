#pragma once

#include <functional>

namespace Twin2Engine {
	namespace UI {
		template<typename T> concept args_t = true;

		template<typename Ret, args_t... Args> using Func = std::function<Ret(Args)>;
		template<args_t... Args> using Action = Func<void, Args>;
		using Method = Action<void>;

		template<args_t... Args>
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