#include <graphic/Texture2D.h>

using namespace Twin2Engine::GraphicEngine;

Texture2D::Texture2D(size_t managerId, unsigned int id, unsigned int width, unsigned int height, unsigned int channelsNum, 
	const TextureFormat& format, const TextureWrapMode& sWrapMode, const TextureWrapMode& tWrapMode, 
	const TextureFilterMode& minFilterMode, const TextureFilterMode& magFilterMode)
	: _managerId(managerId), _id(id), _width(width), _height(height), _channelsNum(channelsNum),
	_format(format), _sWrapMode(sWrapMode), _tWrapMode(tWrapMode),
	_minFilterMode(minFilterMode), _magFilterMode(magFilterMode) {}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &_id);
}

void Texture2D::SetWrapModeS(const TextureWrapMode& mode)
{
	glBindTexture(GL_TEXTURE_2D, _id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
	_sWrapMode = mode;
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::SetWrapModeT(const TextureWrapMode& mode)
{
	glBindTexture(GL_TEXTURE_2D, _id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
	_tWrapMode = mode;
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::SetMinFilterMode(const TextureFilterMode& mode)
{
	glBindTexture(GL_TEXTURE_2D, _id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode);
	_minFilterMode = mode;
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::SetMagFilterMode(const TextureFilterMode& mode)
{
	glBindTexture(GL_TEXTURE_2D, _id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mode);
	_magFilterMode = mode;
	glBindTexture(GL_TEXTURE_2D, 0);
}

size_t Texture2D::GetManagerId() const
{
	return _managerId;
}

unsigned int Texture2D::GetId() const
{
	return _id;
}

unsigned int Texture2D::GetWidth() const
{
	return _width;
}

unsigned int Texture2D::GetHeight() const
{
	return _height;
}

unsigned int Texture2D::GetChannelsNum() const
{
	return _channelsNum;
}

TextureWrapMode Texture2D::GetWrapModeS() const
{
	return _sWrapMode;
}

TextureWrapMode Texture2D::GetWrapModeT() const
{
	return _tWrapMode;
}

TextureFilterMode Texture2D::GetMinFilterMode() const
{
	return _minFilterMode;
}

TextureFilterMode Texture2D::GetMagFilterMode() const
{
	return _magFilterMode;
}

void Texture2D::Use(unsigned int samplerId) const
{
	glActiveTexture(GL_TEXTURE0 + samplerId);
	glBindTexture(GL_TEXTURE_2D, _id);
}