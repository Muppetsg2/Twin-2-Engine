#include <ui/Text.h>
#include <core/Transform.h>
#include <core/YamlConverters.h>
#include <manager/SceneManager.h>
#include <graphic/Shader.h>
#include <graphic/manager/UIRenderingManager.h>
#include <graphic/manager/FontManager.h>

using namespace Twin2Engine;
using namespace UI;
using namespace Core;
using namespace GraphicEngine;
using namespace Manager;
using namespace glm;
using namespace std;

void Text::UpdateTextCache()
{
	Font* font = FontManager::GetFont(_fontId);
	if (_size != 0 && font != nullptr) {
		if (_justResizeCache) {
			// Zastêpowanie
			for (size_t i = 0; i < _oldText.size() && i < _text.size() && i < _textCache.size(); ++i) {
				if (_oldText[i] != _text[i]) {
					_textCache[i] = font->GetCharacter(_text[i], _size);
				}
			}

			// Dodawanie kolejnych
			if (_oldText.size() < _text.size()) {
				vector<Character*> chars = font->GetText(_text.substr(_oldText.size(), _text.size() - _oldText.size() + 1), _size);
				for (auto& c : chars) _textCache.push_back(c);
			}
			// Odejmowanie nadmiaru
			else if (_oldText.size() > _text.size()) {
				for (size_t i = _oldText.size(); i >= _text.size(); --i) _textCache.erase(_textCache.end() - 1);
			}
		}
		else {
			_textCache = font->GetText(_text, _size);
		}
	}
	else {
		_textCache.clear();
	}
	_textCacheDirty = false;
	_justResizeCache = false;
}

void Text::Render()
{
	if (_textCacheDirty) UpdateTextCache();

	UIElement elem{};
	elem.isText = true;
	elem.color = _color;
	elem.spriteOffset = { 0, 0 };

	mat4 model = GetTransform()->GetTransformMatrix();

	// iterate through all characters
	float x = 0.f;
	float y = 0.f;
	float scale = 1.f;
	for (auto& c : _textCache)
	{
		float xpos = x + c->Bearing.x * scale;
		float ypos = y - (c->Size.y - c->Bearing.y) * scale;

		float w = c->Size.x * scale;
		float h = c->Size.y * scale;

		glm::mat4 tempModel = glm::translate(model, glm::vec3(xpos + w / 2.f, ypos + h / 2.f, 0.f));
		elem.elemSize = { w, h };
		elem.textureSize = { w, h };
		elem.spriteSize = { w, h };
		elem.transform = tempModel;
		elem.textureID = c->TextureID;

		UIRenderingManager::Render(elem);

		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (c->Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
}

YAML::Node Text::Serialize() const
{
	YAML::Node node = RenderableComponent::Serialize();
	node["subTypes"].push_back(node["type"].as<string>());
	node["type"] = "Text";
	node["text"] = _text;
	node["color"] = _color;
	node["size"] = _size;
	node["font"] = SceneManager::GetFontSaveIdx(_fontId);
	return node;
}

void Text::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Text##").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {

		string buff = _text;
		// ustawilem 1000 znakow, ale mozna zawsze zrobic INT_MAX z biblioteka limits.h, ale wywala wtedy blad w UpdateTextCache (nie to, ze teraz go nie wyrzuca)
		ImGui::InputText(string("Value##").append(id).c_str(), buff.data(), 1000);

		if (buff != _text) {
			SetText(buff);
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

		if (GetFont() != nullptr) {
			ImGui::Text("Font Name: %s", FontManager::GetFontName(_fontId).c_str());
			ImGui::SameLine();
			if (ImGui::Button(string("Open File##").append(id).c_str())) {
				_fileDialogOpen = true;
				_fileDialogInfo.type = ImGuiFileDialogType_OpenFile;
				_fileDialogInfo.title = "Open File";
				_fileDialogInfo.directoryPath = std::filesystem::path(std::filesystem::current_path().string() + "\\res\\fonts");
			}

			if (ImGui::FileDialog(&_fileDialogOpen, &_fileDialogInfo))
			{
				// Result path in: m_fileDialogInfo.resultPath
				this->SetFont(_fileDialogInfo.resultPath.string());
			}
		}
		else {
			ImGui::Text("Choose File");
			ImGui::SameLine();
			if (ImGui::Button(string("Open File##").append(id).c_str())) {
				_fileDialogOpen = true;
				_fileDialogInfo.type = ImGuiFileDialogType_OpenFile;
				_fileDialogInfo.title = "Open File";
				_fileDialogInfo.directoryPath = std::filesystem::path(std::filesystem::current_path().string() + "\\res\\fonts");
			}

			if (ImGui::FileDialog(&_fileDialogOpen, &_fileDialogInfo))
			{
				// Result path in: m_fileDialogInfo.resultPath
				this->SetFont(_fileDialogInfo.resultPath.string());
			}
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

void Text::SetText(const string& text)
{
	if (_text != text) {
		_oldText = _text;
		_text = text;

		if (!_textCacheDirty) {
			_textCacheDirty = true;
			_justResizeCache = true;
		}
	}
}

void Text::SetSize(uint32_t size)
{
	if (_size != size) {
		_size = size;
		_textCacheDirty = true;
		_justResizeCache = false;
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
		_textCacheDirty = true;
		_justResizeCache = false;
	}
}

vec4 Text::GetColor() const
{
	return _color;
}

string Text::GetText() const
{
	return _text;
}

uint32_t Text::GetSize() const
{
	return _size;
}

size_t Text::GetFontId() const
{
	return _fontId;
}

Font* Text::GetFont() const
{
	return FontManager::GetFont(_fontId);
}