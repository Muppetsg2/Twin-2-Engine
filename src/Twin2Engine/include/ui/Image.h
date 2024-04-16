#pragma once
#include <core/RenderableComponent.h>
#include <graphic/Sprite.h>

namespace Twin2Engine::UI {
	class Image : public Core::RenderableComponent {
	private:
		size_t _spriteId = 0;
		glm::vec4 _color = glm::vec4(1.f);
		float _width = 100.f;
		float _height = 100.f;

	public:
		virtual void Render() override;
		virtual YAML::Node Serialize() const override;

		void SetSprite(const std::string& spriteAlias);
		void SetSprite(size_t spriteId);
		void SetColor(const glm::vec4& color);
		void SetWidth(float width);
		void SetHeight(float height);

		GraphicEngine::Sprite* GetSprite() const;
		glm::vec4 GetColor() const;
		float GetWidth() const;
		float GetHeight() const;
	};
}