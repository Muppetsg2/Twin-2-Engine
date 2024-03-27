#pragma once
#include <core/RenderableComponent.h>
#include <core/Sprite.h>
#include <glm/glm.hpp>

namespace Twin2Engine {
	namespace UI {
		class Image : public Core::RenderableComponent {
		private:
			Core::Sprite* _sprite = nullptr;
			glm::vec4 _color = glm::vec4(1.f);
			float _width = 100.f;
			float _height = 100.f;

		public:
			virtual void Initialize() override;
			virtual void Render(const Core::Window* window) override;
			virtual void OnDestroy() override;

			void SetSprite(Core::Sprite* sprite);
			void SetColor(const glm::vec4& color);
			void SetWidth(float width);
			void SetHeight(float height);

			constexpr Core::Sprite* GetSprite() const;
			glm::vec4 GetColor() const;
			float GetWidth() const;
			float GetHeight() const;
		};
	}
}