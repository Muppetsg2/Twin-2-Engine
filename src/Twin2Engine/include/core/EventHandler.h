#pragma once

namespace Twin2Engine::Core {
	template<typename Ret, typename... Args> using Func = std::function<Ret(Args...)>;
	template<typename... Args> using Action = Func<void, Args...>;
	using Method = Action<>;

	template<typename Ret, typename... Args> using CFunc = Ret(*)(Args...);
	template<typename... Args> using CAction = CFunc<void, Args...>;
	using CMethod = CAction<>;

	template<typename... Args>
	struct Event {
		size_t ID;
		Action<Args...> Action;
	};

	template<typename... Args>
	class EventHandler {
	private:
		std::vector<Event<Args...>> _events = std::vector<Event<Args...>>();
		size_t _freeId = 0;
		std::queue<size_t> _removedIds = std::queue<size_t>();

	public:
		EventHandler() = default;
		virtual ~EventHandler() = default;

		constexpr size_t AddCallback(const Action<Args...>& callback) {
			Event e{
				.ID = 0,
				.Action = callback
			};
			if (_removedIds.size() > 0) {
				e.ID = _removedIds.front();
				_removedIds.pop();
			}
			else {
				e.ID = _freeId++;
			}
			_events.push_back(e);
			return e.ID;
		}

		constexpr size_t AddCallback(const CAction<Args...>& callback) {
			return AddCallback(Action<Args...>([&](Args...) -> void { callback(Args...); }));
		}

		void RemoveCallback(size_t callbackId) {
			for (size_t i = 0; i < _events.size(); ++i) {
				if (_events[i].ID == callbackId) {
					_events.erase(_events.begin() + i);
					_removedIds.push(callbackId);
					break;
				}
			}
		}
		constexpr void RemoveAllCallbacks() {
			_events.clear();
			_freeId = 0;
			while (!_removedIds.empty()) _removedIds.pop();
		}

		void Invoke(Args... args) const {
			for (Event<Args...> event : _events) {
				event.Action(args...);
			}
		}

		constexpr size_t operator+=(const Action<Args...>& callback) {
			return AddCallback(callback);
		}
		constexpr size_t operator+=(const CAction<Args...>& callback) {
			return AddCallback(callback);
		}
		constexpr void operator-=(size_t callbackId) {
			RemoveCallback(callbackId);
		}
		constexpr void operator()(Args... args) const {
			Invoke(args...);
		}
	};
	using MethodEventHandler = EventHandler<>;
}