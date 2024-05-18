#include <graphic/Sprite.h>

using namespace Twin2Engine::Graphic;
using namespace glm;

Sprite::Sprite(size_t managerId, Texture2D* tex, unsigned int xof, unsigned int yof, unsigned int width, unsigned int height)
	: _managerId(managerId), _tex(tex), _offset(xof, yof), _size(width, height) {}

Sprite::Sprite(size_t managerId, Texture2D* tex, uvec2 offset, uvec2 size)
	: _managerId(managerId), _tex(tex), _offset(offset), _size(size) {}

size_t Sprite::GetManagerId() const
{
	return _managerId;
}

Texture2D* Sprite::GetTexture() const
{
	return _tex;
}

uvec2 Sprite::GetOffset() const {
	return _offset;
}

unsigned int Sprite::GetXOffset() const
{
	return _offset.x;
}

unsigned int Sprite::GetYOffset() const
{
	return _offset.y;
}

uvec2 Sprite::GetSize() const {
	return _size;
}

unsigned int Sprite::GetWidth() const
{
	return _size.x;
}

unsigned int Sprite::GetHeight() const
{
	return _size.y;
}

void Sprite::Use(unsigned int samplerId) const
{
	_tex->Use(samplerId);
}