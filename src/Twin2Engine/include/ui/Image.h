#pragma once
#include <graphic/manager/UIRenderingManager.h>
#include <core/RenderableComponent.h>
#include <graphic/Sprite.h>

namespace Twin2Engine::UI {
	class Canvas;

	class Image : public Core::RenderableComponent {
	private:
		size_t _spriteId = 0;
		size_t _onTransformChangeId = 0;
		size_t _onRotationChangeId = 0;
		size_t _onParentInHierarchiChangeId = 0;
		size_t _onCanvasDestroyId = 0;
		Canvas* _canvas = nullptr;
		Manager::UIImageData _data = { 
			nullptr /* canvas */,	
			nullptr /* mask */, 
			{	
				glm::mat4(1.f) /* transform */, 
				glm::vec2(100.f, 100.f) /* size */
			} /* rectTransform */,
			glm::vec4(1.f) /* color */,
			0 /* layer */,
			{
				(uint8_t)Manager::FILL_TYPE::HORIZONTAL, /* type */
				(uint8_t)Manager::HORIZONTAL_FILL_SUBTYPE::LEFT, /* subType */
				0.f, /* offset */
				1.f, /* progress */
				0.f, /* rotation */
				false /* isActive */
			} /* fill */,
			nullptr /* sprite */
		};

		void SetCanvas(Canvas* canvas);

	public:
		virtual void Initialize() override;
		virtual void Render() override;
		virtual void OnDestroy() override;
		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;
		
#if _DEBUG
		virtual void DrawEditor() override;
#endif

		void SetSprite(const std::string& spriteAlias);
		void SetSprite(size_t spriteId);
		void SetColor(const glm::vec4& color);
		void SetWidth(float width);
		void SetHeight(float height);
		void SetLayer(int32_t layer);
		void EnableFill(bool enable);
		void SetFillType(Manager::FILL_TYPE type);
		void SetFillSubType(uint8_t subType);
		void SetFillOffset(float offset);
		void SetFillProgress(float progress);

		Graphic::Sprite* GetSprite() const;
		glm::vec4 GetColor() const;
		float GetWidth() const;
		float GetHeight() const;
		int32_t GetLayer() const;
		bool IsFillEnable() const;
		Manager::FILL_TYPE GetFillType() const;
		uint8_t GetFillSubType() const;
		float GetFillOffset() const;
		float GetFillProgress() const;
	};
}