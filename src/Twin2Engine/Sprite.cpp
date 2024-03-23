#include <core/Sprite.h>

using namespace Twin2Engine::Core;

Sprite::Sprite(Texture2D* tex, unsigned int xof, unsigned int yof, unsigned int width, unsigned int height)
	: _tex(tex), _xof(xof), _yof(yof), _width(width), _height(height) {}

constexpr Texture2D* Sprite::GetTexture() const
{
	return _tex;
}

constexpr unsigned int Sprite::GetXOffset() const
{
	return _xof;
}

constexpr unsigned int Sprite::GetYOffset() const
{
	return _yof;
}

constexpr unsigned int Sprite::GetWidth() const
{
	return _width;
}

constexpr unsigned int Sprite::GetHeight() const
{
	return _height;
}

void Sprite::Use(unsigned int samplerId) const
{
	_tex->Use(samplerId);
}