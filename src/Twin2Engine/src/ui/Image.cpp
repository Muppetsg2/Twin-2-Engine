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

	UIImageData image{};
	image.canvas = nullptr; // Na ten moment tylko na ekranie siê wyœwietla
	image.mask = nullptr; // Na ten moment nie ma maski
	image.layer = _layer;

	image.rectTransform = UIRectData{
		.transform = GetTransform()->GetTransformMatrix(),
		.size = { _width, _height }
	};

	image.sprite = sprite;
	image.color = _color;

	UIRenderingManager::Render(image);
}

YAML::Node Image::Serialize() const
{
	YAML::Node node = RenderableComponent::Serialize();
	node["type"] = "Image";
	node["sprite"] = SceneManager::GetSpriteSaveIdx(_spriteId);
	node["color"] = _color;
	node["width"] = _width;
	node["height"] = _height;
	node["layer"] = _layer;
	return node;
}

bool Image::Deserialize(const YAML::Node& node)
{
	if (!node["sprite"] || !node["color"] || !node["width"] || !node["height"] ||
		!node["layer"] || !RenderableComponent::Deserialize(node)) return false;

	_spriteId = SceneManager::GetSprite(node["sprite"].as<size_t>());
	_color = node["color"].as<glm::vec4>();
	_width = node["width"].as<float>();
	_height = node["height"].as<float>();
	_layer = node["layer"].as<int32_t>();

	return true;
}

#if _DEBUG
void Image::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Image##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {

		Component::DrawInheritedFields();
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

		// TODO: Zaktualizowaæ dodaæ layer
	}
}
#endif

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

void Image::SetLayer(int32_t layer)
{
	_layer = layer;
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

int32_t Image::GetLayer() const
{
	return _layer;
}