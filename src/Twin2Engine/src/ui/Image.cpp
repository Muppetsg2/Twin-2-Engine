#include <ui/Image.h>
#include <core/Transform.h>
#include <tools/YamlConverters.h>
#include <manager/SceneManager.h>
#include <graphic/manager/UIRenderingManager.h>
#include <graphic/manager/SpriteManager.h>
#include <core/CameraComponent.h>

using namespace Twin2Engine;
using namespace UI;
using namespace Core;
using namespace Graphic;
using namespace Manager;
using namespace glm;
using namespace std;

void Image::Render()
{
	Sprite* sprite = SpriteManager::GetSprite(_spriteId);

	UIElement elem{};
	elem.canvasSize = Window::GetInstance()->GetContentSize();
	elem.worldSpaceCanvas = false;

	if (sprite != nullptr) {
		elem.textureID = sprite->GetTexture()->GetId();
		elem.textureSize = { sprite->GetTexture()->GetWidth(), sprite->GetTexture()->GetHeight() };
		elem.spriteSize = { sprite->GetWidth(), sprite->GetHeight() };
		elem.spriteOffset = { sprite->GetXOffset(), sprite->GetYOffset() };
		elem.color = _color;
		elem.elemSize = { _width, _height };
		elem.elemTransform = GetTransform()->GetTransformMatrix();
		elem.hasTexture = true;
		UIRenderingManager::Render(elem);
	}
	else {
		elem.color = _color;
		elem.elemSize = { _width, _height };
		elem.elemTransform = GetTransform()->GetTransformMatrix();
		elem.hasTexture = false;
		UIRenderingManager::Render(elem);
	}
}

YAML::Node Image::Serialize() const
{
	YAML::Node node = RenderableComponent::Serialize();
	node["subTypes"].push_back(node["type"].as<string>());
	node["type"] = "Image";
	node["sprite"] = SceneManager::GetSpriteSaveIdx(_spriteId);
	node["color"] = _color;
	node["width"] = _width;
	node["height"] = _height;
	return node;
}

void Image::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Image##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {

		std::map<size_t, string> spriteNames = SpriteManager::GetAllSpritesNames();

		spriteNames.insert(std::pair(0, "None"));

		if (!spriteNames.contains(_spriteId)) {
			_spriteId = 0;
		}

		if (ImGui::BeginCombo(string("Font##").append(id).c_str(), spriteNames[_spriteId].c_str())) {

			bool clicked = false;
			size_t choosed = _spriteId;
			for (auto& item : spriteNames) {

				if (ImGui::Selectable(item.second.append("##").append(id).c_str(), item.first == _spriteId)) {

					if (clicked) continue;

					choosed = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed != 0) {
					SetSprite(choosed);
				}
				else {
					_spriteId = 0;
				}
			}

			ImGui::EndCombo();
		}

		float v = _width;
		ImGui::DragFloat(string("Width##").append(id).c_str(), &v, 0.1f);

		if (v != _width) {
			SetWidth(v);
		}

		v = _height;
		ImGui::DragFloat(string("Height##").append(id).c_str(), &v, 0.1f);

		if (v != _height) {
			SetHeight(v);
		}

		glm::vec4 c = _color;
		ImGui::ColorEdit4(string("Color##").append(id).c_str(), glm::value_ptr(c));

		if (c != _color) {
			SetColor(c);
		}

		ImGui::Checkbox(string("Transparent##").append(id).c_str(), &_isTransparent);
	}
}

void Image::SetSprite(const std::string& spriteAlias) {
	_spriteId = hash<string>()(spriteAlias);
}

void Image::SetSprite(size_t spriteId)
{
	_spriteId = spriteId;
}

void Image::SetColor(const vec4& color)
{
	if (color.a != 1.f)
		_isTransparent = true;
	_color = color;
}

void Image::SetWidth(float width)
{
	_width = width;
}

void Image::SetHeight(float height)
{
	_height = height;
}

Sprite* Image::GetSprite() const
{
	return SpriteManager::GetSprite(_spriteId);
}

vec4 Image::GetColor() const
{
	return _color;
}

float Image::GetWidth() const
{
	return _width;
}

float Image::GetHeight() const
{
	return _height;
}