#include <core/Texture2D.h>

using namespace Twin2Engine::Core;

Texture2D::Texture2D(unsigned int id, unsigned int width, unsigned int height, unsigned int channelsNum, 
	const TextureFormat& format, const TextureWrapMode& sWrapMode, const TextureWrapMode& tWrapMode, 
	const TextureFilterMode& minFilterMode, const TextureFilterMode& magFilterMode)
	: _id(id), _width(width), _height(height), _channelsNum(channelsNum),
	_format(format), _sWrapMode(sWrapMode), _tWrapMode(tWrapMode),
	_minFilterMode(minFilterMode), _magFilterMode(magFilterMode) {}

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

constexpr unsigned int Texture2D::GetId() const
{
	return _id;
}

constexpr unsigned int Texture2D::GetWidth() const
{
	return _width;
}

constexpr unsigned int Texture2D::GetHeight() const
{
	return _height;
}

constexpr unsigned int Texture2D::GetChannelsNum() const
{
	return _channelsNum;
}

constexpr TextureWrapMode Texture2D::GetWrapModeS() const
{
	return _sWrapMode;
}

constexpr TextureWrapMode Texture2D::GetWrapModeT() const
{
	return _tWrapMode;
}

constexpr TextureFilterMode Texture2D::GetMinFilterMode() const
{
	return _minFilterMode;
}

constexpr TextureFilterMode Texture2D::GetMagFilterMode() const
{
	return _magFilterMode;
}

void Texture2D::Use(unsigned int samplerId) const
{
	glActiveTexture(GL_TEXTURE0 + samplerId);
	glBindTexture(GL_TEXTURE_2D, _id);
}