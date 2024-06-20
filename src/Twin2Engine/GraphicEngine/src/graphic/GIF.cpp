#include <graphic/GIF.h>

using namespace Twin2Engine::Graphic;
using namespace glm;
using namespace std;

GIF::GIF(size_t managerId, const uvec2& size, const vector<float>& delays, const vector<Texture2D*>& textures, const vector<size_t>& frames)
	: _managerId(managerId), _size(size), _delays(delays), _textures(textures), _frames(frames)
{
}

GIF::~GIF()
{
	_delays.clear();
	for (auto& tex : _textures) {
		delete tex;
	}
	_textures.clear();
	_frames.clear();
}

size_t GIF::GetManagerId() const
{
	return _managerId;
}

uvec2 GIF::GetSize() const
{
	return _size;
}

float GIF::GetDelay(size_t frameIdx) const
{
	if (_delays.size() == 0) return 0.f;

	if (frameIdx < 0) frameIdx = 0;
	else if (frameIdx > _delays.size() - 1) frameIdx = _delays.size() - 1;

	return _delays[frameIdx];
}

Texture2D* GIF::GetFrameTexture(size_t frameIdx) const
{
	if (_frames.size() == 0 || _textures.size() == 0) return nullptr;

	if (frameIdx < 0) frameIdx = 0;
	else if (frameIdx > _frames.size() - 1) frameIdx = _frames.size() - 1;

	size_t textureIdx = _frames[frameIdx];

	if (textureIdx < 0) textureIdx = 0;
	else if (textureIdx > _textures.size() - 1) textureIdx = _textures.size() - 1;

	return _textures[textureIdx];
}

size_t GIF::GetFramesCount() const
{
	return _frames.size();
}