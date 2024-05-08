#include <ui/Image.h>
#include <core/Transform.h>
#include <tools/YamlConverters.h>
#include <manager/SceneManager.h>
#include <graphic/manager/UIRenderingManager.h>
#include <graphic/manager/SpriteManager.h>

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

	if (sprite != nullptr) {
		UIElement elem{};
		elem.isText = false;
		elem.textureID = sprite->GetTexture()->GetId();
		elem.textureSize = { sprite->GetTexture()->GetWidth(), sprite->GetTexture()->GetHeight() };
		elem.spriteSize = { sprite->GetWidth(), sprite->GetHeight() };
		elem.spriteOffset = { sprite->GetXOffset(), sprite->GetYOffset() };
		elem.color = _color;
		elem.elemSize = { _width, _height };
		elem.transform = GetTransform()->GetTransformMatrix();
		elem.hasTexture = true;
		UIRenderingManager::Render(elem);
	}
	else {
		UIElement elem{};
		elem.color = _color;
		elem.elemSize = { _width, _height };
		elem.transform = GetTransform()->GetTransformMatrix();
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

void Image::SetSprite(const std::string& spriteAlias) {
	_spriteId = hash<string>()(spriteAlias);
}

void Image::SetSprite(size_t spriteId)
{
	_spriteId = spriteId;
}

void Image::SetColor(const vec4& color)
{
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