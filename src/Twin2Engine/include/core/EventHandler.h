#pragma once

namespace Twin2Engine::Core {
	template<typename Ret, typename... Args> using Func = std::function<Ret(Args...)>;
	template<typename... Args> using Action = Func<void, Args...>;
	using Method = Action<>;

	template<typename... Args>
	class EventHandler {
	private:
		std::vector<Action<Args...>> _actions = std::vector<Action<Args...>>();

	public:
		EventHandler() = default;
		virtual ~EventHandler() = default;

		void AddCallback(const Action<Args...>& callback) {
			_actions.push_back(callback);
		}
		void RemoveCallback(const Action<Args...>& callback) {
			for (size_t i = 0; i < _actions.size(); ++i) {
				if (_actions[i] == callback) {
					_actions.erase(_actions.begin() + i);
					break;
				}
			}
		}
		void RemoveAllCallbacks() {
			_actions.clear();
		}

		void Invoke(Args... args) const {
			for (Action<Args...> action : _actions) {
				action(args...);
			}
		}

		void operator+=(const Action<Args...>& callback) {
			AddCallback(callback);
		}
		void operator-=(const Action<Args...>& callback) {
			RemoveCallback(callback);
		}
		void operator()(Args... args) {
			Invoke(args);
		}
	};
	using MethodEventHandler = EventHandler<>;
}