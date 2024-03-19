#pragma once

#include <core/Component.h>
#include <functional>

namespace Twin2Engine {
	namespace UI {
		template<typename... Args> using Action = std::function<void(Args)>;

		class Button : public Twin2EngineCore::Component {
		private:
			float _width;
			float _height;
			bool _interactable;

			std::vector<Action<>> _onClickActions = std::vector<Action<>>();

		public:
			void SetWidth(float width);
			void SetHeight(float height);
			void SetInteractable(bool interactable);

			void AddOnClickCallback(const Action<>& callback);

			float GetWidth() const;
			float GetHeight() const;
			bool IsInteractable() const;
		};
	}
}