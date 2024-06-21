#pragma once

#include <core/Component.h>
#include <graphic/manager/UIRenderingManager.h>

namespace Twin2Engine::UI {
	class Image;
	class Text;
	class Button;
	class InputField;
	class GIFPlayer;

	class Canvas : public Core::Component {
	private:
		size_t _onTransformChangeId;
		Manager::CanvasData _data{
			{
				glm::mat4(1.f),
				glm::vec2(100.f, 100.f)
			},
			false,
			0
		};
		Tools::EventHandler<Canvas*> _OnCanvasDestory;

		friend class Image;
		friend class Text;
		friend class Button;
		friend class InputField;
		friend class GIFPlayer;

	public:
		void Initialize() override;
		void OnDestroy() override;
		
		YAML::Node Serialize() const override;
		bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
		void DrawEditor() override;
#endif

		void SetInWorldSpace(bool worldSpace);
		void SetSize(const glm::vec2& size);
		void SetLayer(int32_t layer);

		bool IsInWorldSpace() const;
		glm::vec2 GetSize() const;
		int32_t GetLayer() const;
		Tools::EventHandler<Canvas*>& GetOnCanvasDestroy();
	};
}