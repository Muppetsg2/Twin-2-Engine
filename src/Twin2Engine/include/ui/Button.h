#pragma once

#include <core/Component.h>
#include <tools/EventHandler.h>

namespace Twin2Engine::UI {
	class Canvas;

	class Button : public Core::Component {
	private:
		float _width = 0.f;
		float _height = 0.f;
		bool _interactable = true;
		Tools::MethodEventHandler _onClickEvent = Tools::MethodEventHandler();

		size_t _onParentInHierarchiChangeId = 0;
		size_t _onCanvasDestroyId = 0;
		Canvas* _canvas = nullptr;

		void SetCanvas(Canvas* canvas);

	public:
		void SetWidth(float width);
		void SetHeight(float height);
		void SetInteractable(bool interactable);

		float GetWidth() const;
		float GetHeight() const;
		bool IsInteractable() const;
		Tools::MethodEventHandler& GetOnClickEvent();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void OnDestroy() override;
		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;
		
#if _DEBUG
		virtual void DrawEditor() override;
#endif
	};
}