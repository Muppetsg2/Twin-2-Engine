#pragma once
#include <core/RenderableComponent.h>
#include <graphic/Font.h>
#include <glm/glm.hpp>

namespace Twin2Engine::UI {
	class Text : public Core::RenderableComponent {
	private:
		std::string _text = "";
		std::vector<GraphicEngine::Character*> _textTextures = std::vector<GraphicEngine::Character*>();

		std::string _fontPath = "";

		uint32_t _size = 0;

		glm::vec4 _color = glm::vec4(0.f);

		void UpdateTextTextures();

	public:
		virtual void Render() override;

		void SetColor(const glm::vec4& color);
		void SetText(const std::string& text);
		void SetSize(uint32_t size);
		void SetFontPath(const std::string& fontPath);

		glm::vec4 GetColor() const;
		std::string GetText() const;
		uint32_t GetSize() const;
		std::string GetFontPath() const;
	};
}