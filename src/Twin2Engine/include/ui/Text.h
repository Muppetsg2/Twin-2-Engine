#pragma once
#include <core/RenderableComponent.h>
#include <graphic/Font.h>
#include <glm/glm.hpp>

namespace Twin2Engine::UI {
	enum class TextAlignX { LEFT, CENTER, RIGHT };
	enum class TextAlignY { BOTTOM, CENTER, TOP };

	class Text : public Core::RenderableComponent {
	private:
		std::string _text = "";

		size_t _fontId = 0;

		uint32_t _size = 0;

		glm::vec4 _color = glm::vec4(0.f, 0.f, 0.f, 1.f);

		TextAlignX _alignX = TextAlignX::LEFT;
		TextAlignY _alignY = TextAlignY::BOTTOM;

		std::vector<Graphic::Character*> _textCache = std::vector<Graphic::Character*>();

		bool _textCacheDirty = true;
		bool _justResizeCache = false;
		std::string _oldText = "";
		float _totalTextWidth = 0.f;
		float _maxTextHeight = 0.f;
		void UpdateTextCache();

	public:
		virtual void Render() override;
		virtual YAML::Node Serialize() const override;

		void SetColor(const glm::vec4& color);
		void SetText(const std::string& text);
		void SetSize(uint32_t size);
		void SetFont(const std::string& fontPath);
		void SetFont(size_t fontId);
		void SetTextAlignX(const TextAlignX& alignX);
		void SetTextAlignY(const TextAlignY& alignY);

		glm::vec4 GetColor() const;
		std::string GetText() const;
		uint32_t GetSize() const;
		size_t GetFontId() const;
		Graphic::Font* GetFont() const;
		TextAlignX GetTextAlignX() const;
		TextAlignY GetTextAlignY() const;
	};
}