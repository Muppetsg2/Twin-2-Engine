#include <ui/Text.h>
#include <core/Transform.h>
#include <core/CameraComponent.h>
#include <tools/YamlConverters.h>
#include <manager/SceneManager.h>
#include <graphic/Shader.h>
#include <graphic/manager/UIRenderingManager.h>
#include <graphic/manager/FontManager.h>
#include <map>
#include <tools/EventHandler.h>
#include <locale>
#include <codecvt>

using namespace Twin2Engine;
using namespace UI;
using namespace Core;
using namespace Graphic;
using namespace Manager;
using namespace Tools;
using namespace glm;
using namespace std;

void Text::UpdateTextMesh()
{
	Font* font = FontManager::GetFont(_fontId);
	
	bool goodSize = false;

	while (true) {
		_textWidth = 0.f;
		_textHeight = 0.f;
		_textCharCache.clear();
		_displayTextCharCache.clear();

		if (_autoSize && _text.size() == 0) {
			_size = _maxSize;
		}

		if (_size != 0 && font != nullptr) {
			const float lineHeight = _size * 1.2f;
			glm::vec2 currPos = glm::vec2(0.f);
			_textHeight = lineHeight;

			// TO CALCULATE TEXT WIDTH
			float lineWidth = 0.f;

			// TEXT WRAPPING INFO
			bool wasSpaceInLine = false;
			size_t spaceCharIdx = 0;
			size_t lastIdx = 0;

			// TEXT ALIGN X INFO
			size_t lineStartIdx = 0;
			float linePadding = 0.f;

			// NEW LINE FUNC
			Action<size_t> newLine = [&](size_t i) -> void {
				for (size_t idx = lineStartIdx; idx < _textCharCache.size(); ++idx) {
					_displayTextCharCache.push_back(_textCharCache[idx]);
				}

				if (_alignX == TextAlignX::CENTER) {
					for (size_t idx = lineStartIdx; idx < i; ++idx) {
						_displayTextCharCache[idx].position.x += (_width - lineWidth) * .5f - linePadding;
						_displayTextCharCache[idx].cursorPos.x += (_width - lineWidth) * .5f - linePadding;
					}
				}
				else if (_alignX == TextAlignX::RIGHT) {
					for (size_t idx = lineStartIdx; idx < i; ++idx) {
						_displayTextCharCache[idx].position.x += _width - lineWidth - linePadding * 2;
						_displayTextCharCache[idx].cursorPos.x += _width - lineWidth - linePadding * 2;
					}
				}

				currPos.x = 0.f;
				currPos.y -= lineHeight;
				_textHeight += lineHeight;

				if (lineWidth > _textWidth) {
					_textWidth = lineWidth;
				}
				lineWidth = 0.f;

				if (_textWrapping) {
					wasSpaceInLine = false;
					spaceCharIdx = 0;
				}

				lineStartIdx = i;
			};
			Func<bool> autoSizeCheck = [&]() -> bool {
				return !goodSize && _autoSize && lineWidth > _width && _size >= _minSize && _size <= _maxSize;
			};
			for (size_t i = 0; i < _text.size(); ++i) {
				if (_text[i] == '\n') {
					if (autoSizeCheck()) {
						goodSize = false;
						break;
					}
					newLine(i);
					continue;
				}

				if (_text[i] == ' ' && _textWrapping) {
					wasSpaceInLine = true;
					spaceCharIdx = i;
				}

				Character* c;
				if (_textWrapping && i < lastIdx) {
					c = _textCharCache[i].character;
				}
				else {
					c = font->GetCharacter(_text[i], _size);
				}

				if (i == lineStartIdx) {
					linePadding = c->Bearing.x;
				}

				float w = c->Size.x;
				float h = c->Size.y;

				bool goodPos = false;
				glm::vec2 charPos;
				while (!goodPos) {
					charPos = currPos;
					charPos.x += (w - _width) * .5f + c->Bearing.x;

					switch (_alignY) {
					case TextAlignY::BOTTOM:
						charPos.y += h * .5f - _size * .25f - (h - c->Bearing.y);
						break;
					case TextAlignY::TOP:
						charPos.y -= h * .5f - _size * .25f - (h - c->Bearing.y);
						break;
					}

					if (_textWrapping && charPos.x + w * .5f > _width * .5f) {
						if (wasSpaceInLine) {
							lastIdx = i;
							i = spaceCharIdx;
							for (size_t idx = i; idx < lastIdx; ++idx) {
								lineWidth -= _textCharCache[idx].character->Advance >> 6;
							}
							if (autoSizeCheck()) {
								goodSize = false;
								break;
							}
							newLine(i);
							break;
						}
						else {
							if (autoSizeCheck()) {
								goodSize = false;
								break;
							}
							newLine(i);
						}
					}
					else {
						goodPos = true;
					}
				}

				if (!goodPos) {
					if (autoSizeCheck()) {
						break;
					}
					continue;
				}

				if (_textWrapping && i < lastIdx) {
					_textCharCache[i].position = charPos;
				}
				else {
					_textCharCache.push_back({ c, charPos, currPos });
				}

				// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
				currPos.x += c->Advance >> 6; // bitshift by 6 to get value in pixels (2^6 = 64)
				lineWidth += c->Advance >> 6;
			}

			float tempSize = _size;
			if (autoSizeCheck()) {
				tempSize = std::max(std::min(_width / lineWidth * tempSize, (float)_maxSize), (float)_minSize);
			}

			if (!goodSize && _autoSize && tempSize >= _minSize && tempSize <= _maxSize) {
				if (_textWidth < _width && _textWidth > lineWidth) {
					tempSize = std::max(std::min(_width / _textWidth * tempSize, (float)_maxSize), (float)_minSize);
				}
				else if (lineWidth < _width && lineWidth > _textWidth) {
					tempSize = std::max(std::min(_width / lineWidth * tempSize, (float)_maxSize), (float)_minSize);
				}
			}

			uint32_t newSize = std::floor(tempSize);
			if (_size != newSize) {
				_size = newSize;
				goodSize = true;
				continue;
			}

			bool truncated = false;
			if (_overflow == TextOverflow::Truncate || _overflow == TextOverflow::Ellipsis) {
				for (size_t idx = lineStartIdx; idx < _textCharCache.size(); ++idx) {
					vec2 charPos = _textCharCache[idx].position;
					vec2 size = _textCharCache[idx].character->Size;
					if (charPos.x + size.x * .5f <= _width * .5f && charPos.y - size.y * .5f <= _height * .5f && charPos.y + size.y * .5f >= -_height * .5f) {
						_displayTextCharCache.push_back(_textCharCache[idx]);
					}
					else {
						lineWidth -= _textCharCache[idx].character->Advance >> 6;
						truncated = true;
					}
				}
			}
			else if (_overflow == TextOverflow::Masking) {
				for (size_t idx = lineStartIdx; idx < _textCharCache.size(); ++idx) {
					vec2 charPos = _textCharCache[idx].position;
					vec2 size = _textCharCache[idx].character->Size;
					if (charPos.x - size.x * .5f <= _width * .5f && charPos.y - size.y * .5f <= _height * .5f && charPos.y + size.y * .5f >= -_height * .5f) {
						_displayTextCharCache.push_back(_textCharCache[idx]);
					}
					else {
						lineWidth -= _textCharCache[idx].character->Advance >> 6;
					}
				}
			}
			else if (_overflow == TextOverflow::Overflow) {
				for (size_t idx = lineStartIdx; idx < _textCharCache.size(); ++idx) {
					_displayTextCharCache.push_back(_textCharCache[idx]);
				}
			}

			if (truncated && _overflow == TextOverflow::Ellipsis) {
				for (size_t idx = 3; idx >= 1; --idx) {
					// DELETE OLD CHAR
					auto& ch = _displayTextCharCache[_displayTextCharCache.size() - idx];
					lineWidth -= ch.character->Advance >> 6;

					// MAKE NEW DOT
					Character* c = font->GetCharacter('.', _size);

					float w = c->Size.x;
					float h = c->Size.y;

					vec2 cursorPos = _displayTextCharCache[_displayTextCharCache.size() - idx - 1].cursorPos;
					cursorPos.x += _displayTextCharCache[_displayTextCharCache.size() - idx - 1].character->Advance >> 6;

					vec2 charPos = cursorPos;
					charPos.x += (w - _width) * .5f + c->Bearing.x;

					switch (_alignY) {
					case TextAlignY::BOTTOM:
						charPos.y += h * .5f - _size * .25f - (h - c->Bearing.y);
						break;
					case TextAlignY::TOP:
						charPos.y -= h * .5f - _size * .25f - (h - c->Bearing.y);
						break;
					}

					// UPDATE INFO
					ch.position = charPos;
					ch.cursorPos = cursorPos;
					ch.character = c;
				}
			}

			if (_alignX == TextAlignX::CENTER) {
				for (size_t idx = lineStartIdx; idx < _displayTextCharCache.size(); ++idx) {
					_displayTextCharCache[idx].position.x += (_width - lineWidth) * .5f - linePadding;
					_displayTextCharCache[idx].cursorPos.x += (_width - lineWidth) * .5f - linePadding;
				}
			}
			else if (_alignX == TextAlignX::RIGHT) {
				for (size_t idx = lineStartIdx; idx < _displayTextCharCache.size(); ++idx) {
					_displayTextCharCache[idx].position.x += _width - lineWidth - linePadding * 2.f;
					_displayTextCharCache[idx].cursorPos.x += _width - lineWidth - linePadding * 2.f;
				}
			}

			if (lineWidth > _textWidth) {
				_textWidth = lineWidth;
			}
		}

		break;
	}

	_textDirty = false;
}

void Text::Update()
{
	if (_textDirty) {
		UpdateTextMesh();
	}
}

void Text::Render()
{
	UIElement elem{};
	elem.hasTexture = true;
	elem.isText = true;
	elem.color = _color;
	elem.spriteOffset = { 0, 0 };
	elem.canvasSize = Window::GetInstance()->GetContentSize();
	elem.worldSpaceCanvas = false;

	mat4 model = GetTransform()->GetTransformMatrix();
	if (_overflow != TextOverflow::Overflow) {
		elem.maskTransform = model;
		elem.maskSize = { _width, _height };
		elem.useMask = true;
	}

	// iterate through all characters
	for (auto& c : _displayTextCharCache)
	{
		float w = c.character->Size.x;
		float h = c.character->Size.y;

		glm::mat4 tempModel = glm::translate(model, glm::vec3(c.position, 0.f));
		elem.elemSize = { w, h };
		elem.textureSize = { w, h };
		elem.spriteSize = { w, h };
		elem.elemTransform = tempModel;
		elem.textureID = c.character->TextureID;

		UIRenderingManager::Render(elem);
	}
}

YAML::Node Text::Serialize() const
{
	YAML::Node node = RenderableComponent::Serialize();
	node["type"] = "Text";
	node["text"] = string(_text.begin(), _text.end());
	node["font"] = SceneManager::GetFontSaveIdx(_fontId);
	node["size"] = _size;
	node["autoSize"] = _autoSize;
	node["minSize"] = _minSize;
	node["maxSize"] = _maxSize;
	node["width"] = _width;
	node["height"] = _height;
	node["color"] = _color;
	node["alignX"] = _alignX;
	node["alignY"] = _alignY;
	node["textWrapping"] = _textWrapping;
	node["overflow"] = _overflow;
	return node;
}

bool Text::Deserialize(const YAML::Node& node)
{
	if (!node["text"] || !node["font"] || !node["size"] || !node["autoSize"] ||
		!node["minSize"] || !node["maxSize"] || !node["width"] || !node["height"] ||
		!node["color"] || !node["alignX"] || !node["alignY"] || !node["textWrapping"] ||
		!node["overflow"] || !RenderableComponent::Deserialize(node)) return false;

	string temp = node["text"].as<string>();
	_text = wstring(temp.begin(), temp.end());
	_fontId = SceneManager::GetFont(node["font"].as<size_t>());
	_size = node["size"].as<uint32_t>();
	_autoSize = node["autoSize"].as<bool>();
	_minSize = node["minSize"].as<uint32_t>();
	_maxSize = node["maxSize"].as<uint32_t>();
	_width = node["width"].as<float>();
	_height = node["height"].as<float>();
	_color = node["color"].as<vec4>();
	_alignX = node["alignX"].as<TextAlignX>();
	_alignY = node["alignY"].as<TextAlignY>();
	_textWrapping = node["textWrapping"].as<bool>();
	_overflow = node["overflow"].as<TextOverflow>();

	return true;
}

void Text::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Text##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {

		string buff = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(_text);
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll;

		ImGui::InputText(string("Value##").append(id).c_str(), &buff, flags);

		if (buff != std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(_text)) {
			SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(buff));
		}

		glm::vec4 c = _color;
		ImGui::ColorEdit4(string("Color##").append(id).c_str(), glm::value_ptr(c));

		if (c != _color) {
			SetColor(c);
		}

		float s = _size;
		ImGui::DragFloat(string("Size##").append(id).c_str(), &s);

		if (s != _size) {
			SetSize(s);
		}

		std::map<size_t, string> fontNames = FontManager::GetAllFontsNames();

		fontNames.insert(std::pair(0, "None"));

		if (!fontNames.contains(_fontId)) {
			_fontId = 0;
			_textDirty = false;
		}

		if (ImGui::BeginCombo(string("Font##").append(id).c_str(), fontNames[_fontId].c_str())) {
			
			bool clicked = false;
			size_t choosed = _fontId;
			for (auto& item : fontNames) {

				if (ImGui::Selectable(item.second.append("##").append(id).c_str(), item.first == _fontId)) {
					
					if (clicked) continue;

					choosed = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed != 0) {
					SetFont(choosed);
				}
				else {
					_fontId = 0;
					_textDirty = false;
				}
			}

			ImGui::EndCombo();
		}

		ImGui::Checkbox(string("Transparent##").append(id).c_str(), &_isTransparent);
	}
}

void Text::SetColor(const vec4& color)
{
	if (color.a != 1.f)
		_isTransparent = true;
	_color = color;
}

void Text::SetText(const wstring& text)
{
	if (_text != text) {
		_textDirty = true;
		_text = text;
	}
}

void Text::SetSize(uint32_t size)
{
	if (_size != size) {
		_size = size;
		_textDirty = true;
	}
}

void Text::SetWidth(float width)
{
	if (_width != width) {
		_textDirty = true;
		_width = width;
	}
}

void Text::SetHeight(float height)
{
	if (_height != height) {
		_textDirty = true;
		_height = height;
	}
}

void Text::SetFont(const string& fontPath)
{
	size_t h = hash<string>()(fontPath);
	if (FontManager::GetFont(fontPath) == nullptr) {
		FontManager::LoadFont(fontPath);
	}
	SetFont(h);
}

void Text::SetFont(size_t fontId)
{
	if (_fontId != fontId) {
		_fontId = fontId;
		_textDirty = true;
	}
}

void Text::SetTextAlignX(const TextAlignX& alignX)
{
	if (_alignX != alignX) {
		_alignX = alignX;
		_textDirty = true;
	}
}

void Text::SetTextAlignY(const TextAlignY& alignY)
{
	if (_alignY != alignY) {
		_alignY = alignY;
		_textDirty = true;
	}
}

void Text::SetTextWrapping(bool textWrapping)
{
	if (_textWrapping != textWrapping) {
		_textWrapping = textWrapping;
		_textDirty = true;
	}
}

void Text::SetTextOverflow(const TextOverflow& overflow)
{
	if (_overflow != overflow) {
		_overflow = overflow;
		_textDirty = true;
	}
}

void Text::EnableAutoSize(uint32_t minSize, uint32_t maxSize)
{
	if (!_autoSize || _minSize != minSize || _maxSize != maxSize) {
		_autoSize = true;
		_minSize = minSize;
		_maxSize = maxSize;
		_textDirty = true;
	}
}

void Text::DisableAutoSize()
{
	_autoSize = false;
}

vec4 Text::GetColor() const
{
	return _color;
}

wstring Text::GetText() const
{
	return _text;
}

uint32_t Text::GetSize() const
{
	return _size;
}

float Text::GetHeight() const
{
	return _height;
}

float Text::GetWidth() const
{
	return _width;
}

size_t Text::GetFontId() const
{
	return _fontId;
}

Font* Text::GetFont() const
{
	return FontManager::GetFont(_fontId);
}

TextAlignX Text::GetTextAlignX() const
{
	return _alignX;
}

TextAlignY Text::GetTextAlignY() const
{
	return _alignY;
}

bool Text::IsTextWrapping() const
{
	return _textWrapping;
}

TextOverflow Text::GetTextOverflow() const
{
	return _overflow;
}

float Text::GetTextWidth() const
{
	return _textWidth;
}

float Text::GetTextHeight() const
{
	return _textHeight;
}

bool Text::IsAutoSize() const
{
	return _autoSize;
}

uint32_t Text::GetMinSize() const
{
	return _minSize;
}

uint32_t Text::GetMaxSize() const
{
	return _maxSize;
}