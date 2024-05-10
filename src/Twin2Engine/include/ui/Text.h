#pragma once
#include <core/RenderableComponent.h>
#include <graphic/Font.h>
#include <glm/glm.hpp>

namespace Twin2Engine::UI {
	enum class TextAlignX { LEFT = 0, CENTER = 1, RIGHT = 2 };
	enum class TextAlignY { BOTTOM = 0, CENTER = 1, TOP = 2 };
	enum class TextOverflow { Overflow = 0, Masking = 1, Truncate = 2, Ellipsis = 3 };

	struct TextCharacter {
		Graphic::Character* character = nullptr;
		glm::vec2 position = glm::vec2(0.f);
	};

	class Text : public Core::RenderableComponent {
	private:
		std::string _text = "";
		std::string _displayText = "";

		size_t _fontId = 0;

		uint32_t _size = 0; // text pixel height
		bool _autoSize = false;
		uint32_t _minSize = 0;
		uint32_t _maxSize = 0;

		float _displayTextWidth = 0.f;
		float _displayTextHeight = 0.f;
		float _textWidth = 0.f;
		float _textHeight = 0.f;
		float _height = 0.f;
		float _width = 0.f;

		glm::vec4 _color = glm::vec4(0.f, 0.f, 0.f, 1.f);

		TextAlignX _alignX = TextAlignX::LEFT;
		TextAlignY _alignY = TextAlignY::BOTTOM;

		bool _textWrapping = false;

		TextOverflow _overflow = TextOverflow::Overflow;

		bool _textDirty = true;
		bool _displayTextDirty = true;
		std::string _oldText = "";
		std::string _oldDisplayText = "";
		std::vector<TextCharacter> _textCharCache;
		std::vector<TextCharacter> _displayTextCharCache;

		void UpdateTextCharCache(Graphic::Font* font, const std::string& newText, const std::string& oldText, float& textWidth, float& textHeight, std::vector<TextCharacter>& textCharCache);
	public:
		void UpdateTextMesh();

		void Update() override;
		void Render() override;
		YAML::Node Serialize() const override;

		void SetColor(const glm::vec4& color);
		void SetText(const std::string& text);
		void SetSize(uint32_t size);
		void SetWidth(float width);
		void SetHeight(float height);
		void SetFont(const std::string& fontPath);
		void SetFont(size_t fontId);
		void SetTextAlignX(const TextAlignX& alignX);
		void SetTextAlignY(const TextAlignY& alignY);
		void SetTextWrapping(bool textWrapping);
		void SetTextOverflow(const TextOverflow& overflow);

		glm::vec4 GetColor() const;
		std::string GetText() const;
		uint32_t GetSize() const;
		float GetHeight() const;
		float GetWidth() const;
		size_t GetFontId() const;
		Graphic::Font* GetFont() const;
		TextAlignX GetTextAlignX() const;
		TextAlignY GetTextAlignY() const;
		bool IsTextWrapping() const;
		TextOverflow GetTextOverflow() const;
	};
}