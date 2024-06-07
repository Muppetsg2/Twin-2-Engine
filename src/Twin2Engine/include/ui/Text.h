#pragma once
#include <core/RenderableComponent.h>
#include <graphic/manager/UIRenderingManager.h>
#include <graphic/Font.h>
#include <tools/macros.h>

namespace Twin2Engine::UI {
	ENUM_CLASS_VALUE(TextAlignX, LEFT, 0, CENTER, 1, RIGHT, 2)
	ENUM_CLASS_VALUE(TextAlignY, BOTTOM, 0, CENTER, 1, TOP, 2)
	ENUM_CLASS_VALUE(TextOverflow, Overflow, 0, Masking, 1, Truncate, 2, Ellipsis, 3)

	struct TextCharacter {
		Graphic::Character* character = nullptr;
		glm::vec2 position = glm::vec2(0.f);
		glm::vec2 cursorPos = glm::vec2(0.f);
	};

	class InputField;
	class Canvas;

	class Text : public Core::RenderableComponent {
	private:
		std::wstring _text = L"";

		size_t _fontId = 0;

		uint32_t _size = 0; // text pixel height
		bool _autoSize = false;
		uint32_t _minSize = 0;
		uint32_t _maxSize = 0;

		float _textWidth = 0.f;
		float _textHeight = 0.f;
		float _height = 0.f;
		float _width = 0.f;

		glm::vec4 _color = glm::vec4(0.f, 0.f, 0.f, 1.f);

		TextAlignX _alignX = TextAlignX::LEFT;
		TextAlignY _alignY = TextAlignY::BOTTOM;

		bool _textWrapping = false;

		TextOverflow _overflow = TextOverflow::Overflow;
		Manager::MaskData _textMask;

		int32_t _layer = 0;

		bool _textDirty = true;
		std::vector<TextCharacter> _textCharCache;
		std::vector<TextCharacter> _displayTextCharCache;

		size_t _onParentInHierarchiChangeId = 0;
		size_t _onCanvasDestroyId = 0;
		Canvas* _canvas = nullptr;

		friend class InputField;

		void SetCanvas(Canvas* canvas);
	public:
		// TODO: Repair Text Mesh
		void UpdateTextMesh();

		void Initialize() override;
		void Update() override;
		void Render() override;
		void OnDestroy() override;
		YAML::Node Serialize() const override;
		bool Deserialize(const YAML::Node& node) override;
		
#if _DEBUG
		virtual void DrawEditor() override;
#endif

		void SetColor(const glm::vec4& color);
		void SetText(const std::wstring& text);
		void SetSize(uint32_t size);
		void SetWidth(float width);
		void SetHeight(float height);
		void SetFont(const std::string& fontPath);
		void SetFont(size_t fontId);
		void SetTextAlignX(const TextAlignX& alignX);
		void SetTextAlignY(const TextAlignY& alignY);
		void SetTextWrapping(bool textWrapping);
		void SetTextOverflow(const TextOverflow& overflow);
		void SetLayer(int32_t layer);
		void EnableAutoSize(uint32_t minSize, uint32_t maxSize);
		void DisableAutoSize();

		glm::vec4 GetColor() const;
		std::wstring GetText() const;
		uint32_t GetSize() const;
		float GetHeight() const;
		float GetWidth() const;
		size_t GetFontId() const;
		Graphic::Font* GetFont() const;
		TextAlignX GetTextAlignX() const;
		TextAlignY GetTextAlignY() const;
		bool IsTextWrapping() const;
		TextOverflow GetTextOverflow() const;
		int32_t GetLayer() const;
		float GetTextWidth() const;
		float GetTextHeight() const;
		bool IsAutoSize() const;
		uint32_t GetMinSize() const;
		uint32_t GetMaxSize() const;
	};
}

namespace YAML {
	template<> struct convert<Twin2Engine::UI::TextAlignX> {
		using TextAlignX = Twin2Engine::UI::TextAlignX;

		static YAML::Node encode(const TextAlignX& rhs) {
			YAML::Node node;
			node = (size_t)rhs;
			node.SetTag(to_string(rhs));
			return node;
		}

		static bool decode(const YAML::Node& node, TextAlignX& rhs) {
			if (!node.IsScalar()) return false;

			rhs = (TextAlignX)node.as<size_t>();

			return true;
		}
	};

	template<> struct convert<Twin2Engine::UI::TextAlignY> {
		using TextAlignY = Twin2Engine::UI::TextAlignY;

		static YAML::Node encode(const TextAlignY& rhs) {
			YAML::Node node;
			node = (size_t)rhs;
			node.SetTag(to_string(rhs));
			return node;
		}

		static bool decode(const YAML::Node& node, TextAlignY& rhs) {
			if (!node.IsScalar()) return false;

			rhs = (TextAlignY)node.as<size_t>();

			return true;
		}
	};

	template<> struct convert<Twin2Engine::UI::TextOverflow> {
		using TextOverflow = Twin2Engine::UI::TextOverflow;

		static YAML::Node encode(const TextOverflow& rhs) {
			YAML::Node node;
			node = (size_t)rhs;
			node.SetTag(to_string(rhs));
			return node;
		}

		static bool decode(const YAML::Node& node, TextOverflow& rhs) {
			if (!node.IsScalar()) return false;

			rhs = (TextOverflow)node.as<size_t>();

			return true;
		}
	};
}