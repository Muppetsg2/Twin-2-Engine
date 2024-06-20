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
	delete[] temp;
	if (!framesBuffer) {
		SPDLOG_ERROR("GIF Loading from memory Error");
		stbi_image_free(framesBuffer);
		stbi_image_free(delaysMS);
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
	vector<unsigned char*> scanedTextureData;
	vector<Texture2D*> textures;
	vector<size_t> frames;
	vector<float> delaysS;

	size_t frameSize = size.x * size.y * channels;
	for (size_t i = 0; i < framesCount; ++i) {
		bool hasTexture = false;
		size_t idx = textures.size();
		if (scanedTextureData.size() != 0) {
			for (size_t stdI = 0; stdI < scanedTextureData.size(); ++stdI) {
				bool same = true;
				for (size_t dI = 0; dI < frameSize; ++dI) {
					if (scanedTextureData[stdI][dI] != *(framesBuffer + i * frameSize + dI)) {
						same = false;
						break;
					}
				}

				if (same) {
					hasTexture = true;
					idx = stdI;
				}
			}
		}

		if (!hasTexture) {
			// CREATE TEXTURE
			unsigned int texId;
			glGenTextures(1, &texId);
			glActiveTexture(GL_TEXTURE0);

			glBindTexture(GL_TEXTURE_2D, texId);

			TextureWrapMode wrap = TextureWrapMode::CLAMP_TO_EDGE;
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)wrap);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)wrap);

			TextureFilterMode minFilter = TextureFilterMode::LINEAR_MIPMAP_LINEAR;
			TextureFilterMode magFilter = TextureFilterMode::LINEAR;
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)minFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)magFilter);

			glTexImage2D(GL_TEXTURE_2D, 0, (unsigned int)inter, size.x, size.y, 0, (unsigned int)form, GL_UNSIGNED_BYTE, framesBuffer + i * frameSize);
			glGenerateMipmap(GL_TEXTURE_2D);

			textures.push_back(new Texture2D(id, texId, size, (unsigned int)channels, form, wrap, wrap, minFilter, magFilter));
			scanedTextureData.push_back(framesBuffer + i * frameSize);
		}

		// ADD FRAME DATA
		frames.push_back(idx);
		delaysS.push_back(*(delaysMS + i) / 1000.f);
	}

	// CREATE GIF
	GIF* gif = new GIF(id, size, delaysS, textures, frames);

	// SAVE GIF
	_gifs[id] = gif;
	_paths[id] = path;

	// FREE RESOURCES
	scanedTextureData.clear();
	textures.clear();
	frames.clear();
	delaysS.clear();

	stbi_image_free(framesBuffer);
	stbi_image_free(delaysMS);

	// RETURN GIF
	return gif;
}

GIF* GIFManager::Get(size_t id)
{
	if (!_gifs.contains(id)) {
		SPDLOG_ERROR("There is no loaded gif of id '{0}'", id);
		return nullptr;
	}
	return _gifs[id];
}

GIF* GIFManager::Get(const string& path)
{
	size_t id = _hasher(path);
	if (!_gifs.contains(id)) {
		SPDLOG_WARN("Gif '{0}' is not loaded. Started Loading...", path);
		return Load(path);
	}
	return _gifs[id];
}

void GIFManager::Unload(size_t id)
{
	if (!_gifs.contains(id)) {
		SPDLOG_WARN("There is no gif of id '{0}' to unload", id);
		return;
	}

	delete _gifs[id];

	_gifs.erase(id);
	_paths.erase(id);
}

void GIFManager::Unload(const string& path)
{
	size_t id = _hasher(path);
	if (!_gifs.contains(id)) {
		SPDLOG_WARN("Gif '{0}' cannot be unloaded if is not loaded", path);
		return;
	}

	Unload(id);
}

void GIFManager::UnloadAll()
{
	for (auto& gifPair : _gifs) {
		delete gifPair.second;
	}
	
	_gifs.clear();
	_paths.clear();
}

bool GIFManager::IsLoaded(size_t id)
{
	return _gifs.contains(id);
}

bool GIFManager::IsLoaded(const string& path)
{
	return IsLoaded(_hasher(path));
}

string GIFManager::GetName(size_t id)
{
	if (!_paths.contains(id)) return "";
	string p = _paths[id];
	return std::filesystem::path(p).stem().string();
}

string GIFManager::GetName(const string& path)
{
	if (!_paths.contains(_hasher(path))) return "";
	return std::filesystem::path(path).stem().string();
}

#if _DEBUG
string GIFManager::GetPath(size_t id)
{
	if (!_paths.contains(id)) return "";
	return _paths[id];
}
#endif

map<size_t, string> GIFManager::GetAllNames()
{
	std::map<size_t, std::string> names = std::map<size_t, std::string>();

	for (auto item : _paths) {
		names[item.first] = std::filesystem::path(item.second).stem().string();
	}
	return names;
}

YAML::Node GIFManager::Serialize()
{
	YAML::Node gifs;
	size_t id = 0;
	for (const auto& pathPair : _paths) {
		GIF* gif = _gifs[pathPair.first];

		YAML::Node gifNode;
		gifNode["id"] = id++;
		gifNode["path"] = pathPair.second;
		gifs.push_back(gifNode);
	}
	return gifs;
}

#if _DEBUG
void GIFManager::DrawEditor(bool* p_open)
{
    if (!ImGui::Begin("GIF Manager", p_open)) {
        ImGui::End();
        return;
    }

    ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    bool node_open = ImGui::TreeNodeEx(string("GIFS##GIF Manager").c_str(), node_flag);

    std::list<size_t> clicked = std::list<size_t>();
    clicked.clear();
    if (node_open) {
        int i = 0;
        for (auto& item : _paths) {
            string n = GetName(item.second);
            ImGui::BulletText(n.c_str());
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 10);
            if (ImGui::Button(string(ICON_FA_TRASH_CAN "##Remove GIF Manager").append(std::to_string(i)).c_str())) {
                clicked.push_back(item.first);
            }
            ++i;
        }
        ImGui::TreePop();
    }

    if (clicked.size() > 0) {
        clicked.sort();

        for (int i = clicked.size() - 1; i > -1; --i)
        {
            Unload(clicked.back());

            clicked.pop_back();
        }
    }
    clicked.clear();

    if (ImGui::Button("Load GIF##GIF Manager", ImVec2(ImGui::GetContentRegionAvail().x, 0.f))) {
        _fileDialogOpen = true;
        _fileDialogInfo.type = ImGuiFileDialogType_OpenFile;
        _fileDialogInfo.title = "Open File##GIF Manager";
        _fileDialogInfo.directoryPath = std::filesystem::path(std::filesystem::current_path().string() + "\\res\\gifs");
    }

    if (ImGui::FileDialog(&_fileDialogOpen, &_fileDialogInfo))
    {
		string path = std::filesystem::relative(_fileDialogInfo.resultPath).string();
		if (std::regex_search(path, std::regex("(?:[/\\\\]res[/\\\\])"))) {
			path = path.substr(path.find("res"));
		}

		Load(path);
    }

    ImGui::End();
}
#endif