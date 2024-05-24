#pragma once

#include <core/Component.h>
#include <tools/EventHandler.h>

namespace Twin2Engine::UI {
	class Button : public Core::Component {
	private:
		float _width = 0.f;
		float _height = 0.f;
		bool _interactable = true;
		Tools::MethodEventHandler _onClickEvent = Tools::MethodEventHandler();

	public:
		void SetWidth(float width);
		void SetHeight(float height);
		void SetInteractable(bool interactable);

		float GetWidth() const;
		float GetHeight() const;
		bool IsInteractable() const;
		Tools::MethodEventHandler& GetOnClickEvent();

		virtual void Update() override;
		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;
		
#if _DEBUG
		virtual void DrawEditor() override;
#endif
	};
}