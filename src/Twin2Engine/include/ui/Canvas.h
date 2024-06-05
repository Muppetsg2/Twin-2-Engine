#pragma once

#include <core/Component.h>
#include <graphic/manager/UIRenderingManager.h>

namespace Twin2Engine::UI {
	class Image;
	class Text;

	// TODO: add Canvas To Text
	// TODO: add Canvas To Image
	// TODO: add Canvas To Button
	// TODO: add Canvas To InputField
	class Canvas : public Core::Component {
	private:
		size_t _onTransformChangeId;
		Manager::CanvasData _data{
			{
				glm::mat4(1.f),
				glm::vec2(100.f, 100.f)
			},
			false
		};
		Tools::EventHandler<Canvas*> _OnCanvasDestory;

		friend class Image;

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

		bool IsInWorldSpace() const;
		glm::vec2 GetSize() const;
		Tools::EventHandler<Canvas*>& GetOnCanvasDestroy();
	};
}