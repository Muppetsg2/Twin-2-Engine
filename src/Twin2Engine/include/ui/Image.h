#pragma once
#include <core/RenderableComponent.h>
#include <graphic/Sprite.h>

namespace Twin2Engine::UI {
	class Image : public Core::RenderableComponent {
	private:
		std::string _spriteAlias = "";
		glm::vec4 _color = glm::vec4(1.f);
		float _width = 100.f;
		float _height = 100.f;

	public:
		virtual void Render() override;

		void SetSprite(const std::string& spriteAlias);
		void SetColor(const glm::vec4& color);
		void SetWidth(float width);
		void SetHeight(float height);

		std::string GetSpriteAlias() const;
		GraphicEngine::Sprite* GetSprite() const;
		glm::vec4 GetColor() const;
		float GetWidth() const;
		float GetHeight() const;
	};
}