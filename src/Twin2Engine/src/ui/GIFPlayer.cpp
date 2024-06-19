#include <ui/GIFPlayer.h>
#include <core/Transform.h>
#include <iostream>
#include <core/Time.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::UI;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Graphic;
using namespace std;

void GIFPlayer::Initialize()
{
	ifstream gifFile("res/gifs/Alice-Idle-260x560_2.gif", ios_base::binary);
	if (!gifFile.is_open()) {
		SPDLOG_ERROR("GIF Loading from file Error");
		gifFile.close();
		return;
	}

	gifFile.seekg(0, ios_base::end);
	size_t length = gifFile.tellg();
	gifFile.seekg(0, ios_base::beg);

	char* temp = new char[length];
	gifFile.read(temp, length);
	gifFile.close();
	
	int channels = 0;
	_buffer = stbi_load_gif_from_memory((const unsigned char*)temp, length, &_delays, &_width, &_height, &_frames, &channels, 0);
	if (!_buffer) {
		SPDLOG_ERROR("GIF Loading from memory Error");
		stbi_image_free(_buffer);
		return;
	}

	TextureFormat form = TextureFormat::RGB;
	TextureFileFormat inter = TextureFileFormat::SRGB;
	if (channels == 1) { form = TextureFormat::RED; inter = TextureFileFormat::RED; }
	else if (channels == 2) { form = TextureFormat::RG; inter = TextureFileFormat::RG; }
	else if (channels == 3) { form = TextureFormat::RGB; inter = TextureFileFormat::SRGB; }
	else if (channels == 4) { form = TextureFormat::RGBA; inter = TextureFileFormat::SRGBA; }

	_ids = new unsigned int[_frames];
	_textures = new Texture2D*[_frames];
	glGenTextures(_frames, _ids);
	glActiveTexture(GL_TEXTURE0);

	for (size_t i = 0; i < _frames; ++i) {
		glBindTexture(GL_TEXTURE_2D, _ids[i]);

		TextureWrapMode wrap = TextureWrapMode::CLAMP_TO_EDGE;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)wrap);
		
		TextureFilterMode minFilter = TextureFilterMode::LINEAR_MIPMAP_LINEAR;
		TextureFilterMode magFilter = TextureFilterMode::LINEAR;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)magFilter);

		glTexImage2D(GL_TEXTURE_2D, 0, (unsigned int)inter, _width, _height, 0, (unsigned int)form, GL_UNSIGNED_BYTE, _buffer + i * _width * _height * channels);
		glGenerateMipmap(GL_TEXTURE_2D);

		_textures[i] = new Texture2D(0, _ids[i], _width, _height, (unsigned int)channels, form, wrap, wrap, minFilter, magFilter);
	}
}

void GIFPlayer::Update()
{
	_currTime += Time::GetDeltaTime();
	if (_currTime >= _delays[_imgIdx] / 1000.f) {
		_currTime = 0.f;
		_imgIdx = (_imgIdx + 1) % _frames;
		SPDLOG_ERROR("New frame {0}", _imgIdx);
	}
}

void GIFPlayer::Render()
{
	if (_textures != nullptr)
		UIRenderingManager::Render(UIRectData { .transform = GetTransform()->GetTransformMatrix(), .size = { _width, _height }}, _textures[_imgIdx]);
}

void GIFPlayer::OnDestroy()
{
	// TODO: Destroy
}

YAML::Node GIFPlayer::Serialize() const
{
	return RenderableComponent::Serialize();
}

bool GIFPlayer::Deserialize(const YAML::Node& node)
{
	return RenderableComponent::Deserialize(node);
}

#if _DEBUG
void GIFPlayer::DrawEditor()
{
}
#endif