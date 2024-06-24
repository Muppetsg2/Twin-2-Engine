#pragma once

#include <core/Component.h>
#include <graphic/manager/UIRenderingManager.h>
#include <tools/EventHandler.h>

// TODO: Fix Mask
namespace Twin2Engine::UI {
	class Image;
	class GIFPlayer;

	class Mask : public Core::Component {
	private:
		size_t _spriteId;
		size_t _onTransformChangeId = 0;
		size_t _onRotationChangeId = 0;
		size_t _onParentInHierarchiChangeId = 0;
		Manager::MaskData _data{
			{
				glm::mat4(1.f),
				glm::vec2(100.f, 100.f)
			},
			{
				(uint8_t)Manager::FILL_TYPE::HORIZONTAL,
				(uint8_t)Manager::HORIZONTAL_FILL_SUBTYPE::LEFT,
				0.f,
				1.f,
				0.f,
				false,
			},
			nullptr
		};
		Tools::EventHandler<Mask*> _OnMaskDestory;

		friend class Image;
		friend class GIFPlayer;

	public:
		virtual void Initialize() override;
		virtual void OnDestroy() override;
		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
		virtual void DrawEditor() override;
#endif

		void SetSprite(const std::string& spriteAlias);
		void SetSprite(size_t spriteId);
		void SetWidth(float width);
		void SetHeight(float height);
		void EnableFill(bool enable);
		void SetFillType(Manager::FILL_TYPE type);
		void SetFillSubType(uint8_t subType);
		void SetFillOffset(float offset);
		void SetFillProgress(float progress);

		Graphic::Sprite* GetSprite() const;
		float GetWidth() const;
		float GetHeight() const;
		bool IsFillEnable() const;
		Manager::FILL_TYPE GetFillType() const;
		uint8_t GetFillSubType() const;
		float GetFillOffset() const;
		float GetFillProgress() const;
		Tools::EventHandler<Mask*>& GetOnMaskDestroy();
	};
}