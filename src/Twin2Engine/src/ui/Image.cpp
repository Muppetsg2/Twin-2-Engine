#include <ui/Image.h>
#include <core/Transform.h>
#include <graphic/manager/UIRenderingManager.h>

using namespace Twin2Engine;
using namespace UI;
using namespace Core;
using namespace GraphicEngine;
using namespace Manager;
using namespace glm;
using namespace std;

void Image::Render()
{
	UIElement elem{};
	elem.isText = false;
	elem.textureID = _sprite->GetTexture()->GetId();
	elem.textureSize = { _sprite->GetTexture()->GetWidth(), _sprite->GetTexture()->GetHeight() };
	elem.spriteSize = { _sprite->GetWidth(), _sprite->GetHeight() };
	elem.spriteOffset = { _sprite->GetXOffset(), _sprite->GetYOffset() };
	elem.color = _color;
	elem.elemSize = { _width, _height };
	elem.transform = GetTransform()->GetTransformMatrix();
	UIRenderingManager::Render(elem);
}

void Image::OnDestroy()
{
	_sprite = nullptr;
}

void Image::SetSprite(Sprite* sprite)
{
	_sprite = sprite;
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
	return _sprite;
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