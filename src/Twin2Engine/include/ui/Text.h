#pragma once
#include <core/RenderableComponent.h>
#include <graphic/Font.h>
#include <glm/glm.hpp>

namespace Twin2Engine::UI {
	class Text : public Core::RenderableComponent {
	private:
		std::string _text = "";

		std::string _fontPath = "";

		uint32_t _size = 0;

		glm::vec4 _color = glm::vec4(0.f);

		std::vector<GraphicEngine::Character*> _textCache = std::vector<GraphicEngine::Character*>();

		bool _textCacheDirty = true;
		bool _justResizeCache = false;
		std::string _oldText = "";
		void UpdateTextCache();

	public:
		virtual void Render() override;

		void SetColor(const glm::vec4& color);
		void SetText(const std::string& text);
		void SetSize(uint32_t size);
		void SetFont(const std::string& fontPath);

		glm::vec4 GetColor() const;
		std::string GetText() const;
		uint32_t GetSize() const;
		std::string GetFontPath() const;
		GraphicEngine::Font* GetFont() const;
	};
}