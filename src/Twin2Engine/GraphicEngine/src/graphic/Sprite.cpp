#include <graphic/Sprite.h>

using namespace Twin2Engine::Graphic;

Sprite::Sprite(size_t managerId, Texture2D* tex, unsigned int xof, unsigned int yof, unsigned int width, unsigned int height)
	: _managerId(managerId), _tex(tex), _xof(xof), _yof(yof), _width(width), _height(height) {}

size_t Sprite::GetManagerId() const
{
	return _managerId;
}

Texture2D* Sprite::GetTexture() const
{
	return _tex;
}

unsigned int Sprite::GetXOffset() const
{
	return _xof;
}

unsigned int Sprite::GetYOffset() const
{
	return _yof;
}

unsigned int Sprite::GetWidth() const
{
	return _width;
}

unsigned int Sprite::GetHeight() const
{
	return _height;
}

void Sprite::Use(unsigned int samplerId) const
{
	_tex->Use(samplerId);
}