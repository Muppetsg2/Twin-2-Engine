#include <graphic/manager/GIFManager.h>

using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;
using namespace std;
using namespace glm;

hash<string> GIFManager::_hasher;
unordered_map<size_t, GIF*> GIFManager::_gifs;
unordered_map<size_t, string> GIFManager::_paths;

#if _DEBUG
// For ImGui
bool GIFManager::_fileDialogOpen = false;
ImFileDialogInfo GIFManager::_fileDialogInfo;
#endif

GIF* GIFManager::Load(const string& path)
{
	// CHECK IF GIF IS LOADED
	size_t id = _hasher(path);
	if (_gifs.contains(id)) {
		SPDLOG_WARN("GIF '{0}' already loaded", path);
		return _gifs[id];
	}

	// OPEN FILE
	ifstream gifFile(path, ios_base::binary);
	if (!gifFile.is_open()) {
		SPDLOG_ERROR("Open gif file error");
		gifFile.close();
		return nullptr;
	}

	// CALC FILE LENGTH
	gifFile.seekg(0, ios_base::end);
	size_t length = gifFile.tellg();
	gifFile.seekg(0, ios_base::beg);

	// READ FILE BYTES
	char* temp = new char[length];
	gifFile.read(temp, length);
	gifFile.close();

	// READ GIF DATA
	unsigned char* framesBuffer;
	unsigned int* delaysMS;
	uvec2 size;
	unsigned int framesCount;
	unsigned int channels;
	framesBuffer = stbi_load_gif_from_memory((const unsigned char*)temp, length, (int**)&delaysMS, (int*)&size.x, (int*)&size.y, (int*)&framesCount, (int*)&channels, 0);
	if (!framesBuffer) {
		SPDLOG_ERROR("GIF Loading from memory Error");
		stbi_image_free(framesBuffer);
		return nullptr;
	}

	// AUTO SET FORMAT
	TextureFormat form = TextureFormat::RGB;
	TextureFileFormat inter = TextureFileFormat::SRGB;
	if (channels == 1) { form = TextureFormat::RED; inter = TextureFileFormat::RED; }
	else if (channels == 2) { form = TextureFormat::RG; inter = TextureFileFormat::RG; }
	else if (channels == 3) { form = TextureFormat::RGB; inter = TextureFileFormat::SRGB; }
	else if (channels == 4) { form = TextureFormat::RGBA; inter = TextureFileFormat::SRGBA; }

	// CREATE UNIQUE TEXTURES
	vector<Texture2D*> textures;
	vector<size_t> frames;
	vector<float> delaysS;

	for (size_t i = 0; i < framesCount; ++i) {

	}

	_textures = new Texture2D * [_frames];
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

GIF* GIFManager::Get(size_t id)
{
	return nullptr;
}

GIF* GIFManager::Get(const string& path)
{
	return nullptr;
}

void GIFManager::Unload(size_t id)
{
}

void GIFManager::Unload(const string& path)
{
}

void GIFManager::UnloadAll()
{
}

bool GIFManager::IsLoaded(size_t id)
{
	return false;
}

bool GIFManager::IsLoaded(const string& path)
{
	return false;
}

string GIFManager::GetName(size_t id)
{
	return std::string();
}

string GIFManager::GetName(const string& path)
{
	return std::string();
}

#if _DEBUG
string GIFManager::GetPath(size_t id)
{
	return std::string();
}
#endif

map<size_t, string> GIFManager::GetAllNames()
{
	return std::map<size_t, std::string>();
}

YAML::Node GIFManager::Serialize()
{
	return YAML::Node();
}

#if _DEBUG
void GIFManager::DrawEditor(bool* p_open)
{
}
#endif