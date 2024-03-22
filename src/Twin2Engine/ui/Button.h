#pragma once

#include <core/Component.h>
#include "EventHandler.h"

namespace Twin2Engine {
	namespace UI {
		class Button : public Twin2Engine::Core::Component {
		private:
			float _width = 0.f;
			float _height = 0.f;
			bool _interactable = true;
			MethodEventHandler _onClickEvent = MethodEventHandler();
			bool _notHold = true;

		public:
			void SetWidth(float width);
			void SetHeight(float height);
			void SetInteractable(bool interactable);

			float GetWidth() const;
			float GetHeight() const;
			bool IsInteractable() const;
			const MethodEventHandler& GetOnClickEvent() const;

			void Update() override;
		};
	}
}