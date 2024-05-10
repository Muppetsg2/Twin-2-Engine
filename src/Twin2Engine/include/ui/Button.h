#pragma once

#include <core/Component.h>
#include <core/EventHandler.h>

namespace Twin2Engine::UI {
	class Button : public Core::Component {
	private:
		float _width = 0.f;
		float _height = 0.f;
		bool _interactable = true;
		Core::MethodEventHandler _onClickEvent = Core::MethodEventHandler();
		bool _notHold = true;

	public:
		void SetWidth(float width);
		void SetHeight(float height);
		void SetInteractable(bool interactable);

		float GetWidth() const;
		float GetHeight() const;
		bool IsInteractable() const;
		const Core::MethodEventHandler& GetOnClickEvent() const;

		virtual void Update() override;
		virtual YAML::Node Serialize() const override;
		virtual void DrawEditor() override;
	};
}