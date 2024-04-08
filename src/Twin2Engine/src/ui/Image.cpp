#include <ui/Image.h>
#include <core/Transform.h>
#include <graphic/manager/UIRenderingManager.h>
#include <graphic/manager/SpriteManager.h>

using namespace Twin2Engine;
using namespace UI;
using namespace Core;
using namespace GraphicEngine;
using namespace Manager;
using namespace glm;
using namespace std;

void Image::Render()
{
	Sprite* sprite = SpriteManager::GetSprite(_spriteAlias);

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
		UIRenderingManager::Render(elem);
	}
}

void Image::SetSprite(const std::string& spriteAlias) {
	_spriteAlias = spriteAlias;
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

string Image::GetSpriteAlias() const
{
	return _spriteAlias;
}

Sprite* Image::GetSprite() const
{
	return SpriteManager::GetSprite(_spriteAlias);
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