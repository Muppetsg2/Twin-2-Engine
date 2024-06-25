#include <graphic/manager/SpriteManager.h>
#include <graphic/manager/TextureManager.h>

using namespace Twin2Engine;
using namespace Graphic;
using namespace Manager;
using namespace std;

hash<string> SpriteManager::_hasher;
map<size_t, Sprite*> SpriteManager::_sprites;

map<size_t, string> SpriteManager::_spriteAliases;

#if _DEBUG
bool SpriteManager::_editNext = false;
#endif

void SpriteManager::UnloadSprite(size_t spriteId) {
    if (!_sprites.contains(spriteId)) return;
    delete _sprites[spriteId];
    _sprites.erase(spriteId);
    _spriteAliases.erase(spriteId);
}

void SpriteManager::UnloadSprite(const string& spriteAlias) {
    UnloadSprite(_hasher(spriteAlias));
}

#if _DEBUG
void SpriteManager::DrawSpriteCreator(bool* p_open) {

	if (!ImGui::Begin("Sprite Creator##Sprite Manager", p_open)) {
		ImGui::End();
		return;
	}

	static SpriteData data{ 0, 0, 1, 1 };
	static Texture2D* tex = nullptr;
	static ImTextureID im_id = 0;
	static float w = 1.f;
	static float h = 1.f;

	ImGuiStyle& style = ImGui::GetStyle();
	float alignment = 0.5f;

	float size = 100 + style.FramePadding.x * 2.0f;
	float avail = ImGui::GetContentRegionAvail().x;

	float off = (avail - size) * alignment;
	if (off > 0.0f)
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

	if (ImGui::BeginChild("Preview##SPRITE_CREATOR Sprite Manager", ImVec2(100, 100))) {

		if (tex != nullptr) {
			im_id = (ImTextureID)tex->GetId();
			w = (float)tex->GetWidth();
			h = (float)tex->GetHeight();
		}
		else {
			im_id = 0;
			w = 1.f;
			h = 1.f;
		}
		ImGui::Image(im_id, ImGui::GetContentRegionAvail(), ImVec2((float)data.x / w, (float)data.y / h), ImVec2((float)(data.x + data.width) / w, (float)(data.y + data.height) / h));

		ImGui::EndChild();
	}

	static string buff = "New Sprite";
	string buffTemp = buff;
	ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll;
	vector<string> names = vector<string>();

	for (auto item : GetAllSpritesNames()) {
		names.push_back(item.second);
	}

	static bool error = std::find(names.begin(), names.end(), buffTemp) != names.end();

	if (ImGui::InputText("Name##SPRITE_CREATOR Sprite Manager", &buffTemp, flags)) {
		if (buffTemp != buff) {
			if (buffTemp.size() == 0 || std::find(names.begin(), names.end(), buffTemp) != names.end()) {
				error = true;
			}
			else {
				error = false;
			}
			buff = buffTemp;
		}
	}

	names.clear();

	if (error) ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Incorect Name or Name already exist!");

	map<size_t, string> textures = TextureManager::GetAllTexture2DNames();

	vector<std::pair<size_t, string>> texs = vector<std::pair<size_t, string>>(textures.begin(), textures.end());

	textures.clear();

	vector<string> texNames = vector<string>();
	vector<size_t> ids = vector<size_t>();
	texNames.resize(texs.size());
	ids.resize(texs.size());

	std::sort(texs.begin(), texs.end(), [&](std::pair<size_t, string> const& left, std::pair<size_t, string> const& right) -> bool {
		return left.second.compare(right.second) < 0;
		});

	std::transform(texs.begin(), texs.end(), texNames.begin(), [](std::pair<size_t, string> const& i) -> string {
		return i.second + "##SPRITE_CREATOR Sprite Manager" + std::to_string(i.first);
		});

	std::transform(texs.begin(), texs.end(), ids.begin(), [](std::pair<size_t, string> const& i) -> size_t {
		return i.first;
		});

	texs.clear();

	static size_t selectedTexture = 0;

	int choosed = -1;
	if (selectedTexture != 0) choosed = std::find(ids.begin(), ids.end(), selectedTexture) - ids.begin();

	if (ImGui::ComboWithFilter(string("Texture##SPRITE_CREATOR Sprite Manager").c_str(), &choosed, texNames, 10)) {
		if (choosed != -1) {
			tex = TextureManager::GetTexture2D(ids[choosed]);
			selectedTexture = ids[choosed];

			if (tex != nullptr) {
				if (data.x > tex->GetWidth()) data.x = 0;
				if (data.y > tex->GetHeight()) data.y = 0;
				if (data.x + data.width > tex->GetWidth() || data.width == 1) data.width = tex->GetWidth() - data.x;
				if (data.y + data.height > tex->GetHeight() || data.height == 1) data.height = tex->GetHeight() - data.y;
			}
			else {
				data.x = 0;
				data.y = 0;
				data.width = 1;
				data.height = 1;
			}
		}
	}

	texNames.clear();
	ids.clear();
	/*
	static size_t selectedTexture = 0;
	bool clicked = false;

	if (ImGui::BeginCombo(string("Texture##SPRITE_CREATOR Sprite Manager").c_str(), selectedTexture == 0 ? "None" : textures[selectedTexture].c_str())) {

		size_t i = 0;
		for (auto& item : textures)
		{
			if (ImGui::Selectable(std::string(item.second).append("##SPRITE_CREATOR Sprite Manager").append(std::to_string(i)).c_str(), selectedTexture == item.first))
			{
				if (clicked) {
					continue;
				}
				selectedTexture = item.first;
				clicked = true;

				tex = TextureManager::GetTexture2D(selectedTexture);

				if (tex != nullptr) {
					if (data.x > tex->GetWidth()) data.x = 0;
					if (data.y > tex->GetHeight()) data.y = 0;
					if (data.x + data.width > tex->GetWidth() || data.width == 1) data.width = tex->GetWidth() - data.x;
					if (data.y + data.height > tex->GetHeight() || data.height == 1) data.height = tex->GetHeight() - data.y;
				}
				else {
					data.x = 0;
					data.y = 0;
					data.width = 1;
					data.height = 1;
				}
			}
			++i;
		}
		ImGui::EndCombo();
	}
	*/

	ImGui::BeginDisabled(selectedTexture == 0 || tex == nullptr);
	ImGui::DragUInt("X##SPRITE_CREATOR Sprite Manager", &data.x, 1.f, 0, tex != nullptr ? tex->GetWidth() - 1 : 0);
	ImGui::DragUInt("Y##SPRITE_CREATOR Sprite Manager", &data.y, 1.f, 0, tex != nullptr ? tex->GetHeight() - 1 : 0);
	ImGui::EndDisabled();

	if (tex != nullptr) {
		if (data.x + data.width > tex->GetWidth()) data.width = tex->GetWidth() - data.x;
		if (data.y + data.height > tex->GetHeight()) data.height = tex->GetHeight() - data.y;
	}

	ImGui::BeginDisabled(selectedTexture == 0 || tex == nullptr || (tex != nullptr ? tex->GetWidth() - data.x == 1 : false));
	ImGui::DragUInt("Width##SPRITE_CREATOR Sprite Manager", &data.width, 1.f, 1, tex != nullptr ? tex->GetWidth() - data.x : 0);
	ImGui::EndDisabled();

	ImGui::BeginDisabled(selectedTexture == 0 || tex == nullptr || (tex != nullptr ? tex->GetHeight() - data.y == 1 : false));
	ImGui::DragUInt("Height##SPRITE_CREATOR Sprite Manager", &data.height, 1.f, 1, tex != nullptr ? tex->GetHeight() - data.y : 0);
	ImGui::EndDisabled();

	ImGui::BeginDisabled(error || selectedTexture == 0 || data.width == 0 || data.height == 0);
	if (ImGui::Button("Create##SPRITE_CREATOR Sprite Manager", ImVec2(ImGui::GetContentRegionAvail().x, 0.f))) {
		MakeSprite(buff, selectedTexture, data);
		*p_open = false;
	}
	ImGui::EndDisabled();

	ImGui::End();
}

void SpriteManager::DrawSpriteEditor(bool* p_open, size_t spriteToEdit) {

	if (!ImGui::Begin("Sprite Editor##Sprite Manager", p_open)) {
		ImGui::End();
		return;
	}

	// Sprite
	static Sprite* toEdit = _sprites[spriteToEdit];
	static string buff = _spriteAliases[spriteToEdit];
	static SpriteData data { toEdit->GetXOffset(), toEdit->GetYOffset(), toEdit->GetWidth(), toEdit->GetHeight() };

	// Tex Data
	static Texture2D* tex = nullptr;
	static ImTextureID im_id = 0;
	static float w = 1.f;
	static float h = 1.f;
	static size_t selectedTexture = 0;

	if (_editNext) {

		bool tex_loaded = TextureManager::IsTextureLoaded(_sprites[spriteToEdit]->GetTexture()->GetManagerId());

		toEdit = _sprites[spriteToEdit];
		buff = _spriteAliases[spriteToEdit];

		data.x = tex_loaded ? toEdit->GetXOffset() : 0;
		data.y = tex_loaded ? toEdit->GetYOffset() : 0;
		data.width = tex_loaded ? toEdit->GetWidth() : 0;
		data.height = tex_loaded ? toEdit->GetHeight() : 0;

		tex = tex_loaded ? toEdit->GetTexture() : nullptr;
		im_id = tex_loaded ? (ImTextureID)tex->GetId() : 0;
		w = tex_loaded ? (float)tex->GetWidth() : 1.f;
		h = tex_loaded ? (float)tex->GetHeight() : 1.f;
		selectedTexture = tex_loaded ? tex->GetManagerId() : 0;

		_editNext = false;
	}

	if (!TextureManager::IsTextureLoaded(selectedTexture))
	{
		selectedTexture = 0;
		tex = nullptr;
		im_id = 0;
		w = 1.f;
		h = 1.f;
		data.x = 0;
		data.y = 0;
		data.width = 1;
		data.height = 1;
	}

	ImGuiStyle& style = ImGui::GetStyle();
	float alignment = 0.5f;

	float size = 100 + style.FramePadding.x * 2.0f;
	float avail = ImGui::GetContentRegionAvail().x;

	float off = (avail - size) * alignment;
	if (off > 0.0f)
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

	if (ImGui::BeginChild("Preview##SPRITE_EDITOR Sprite Manager", ImVec2(100, 100))) {

		if (tex != nullptr) {
			im_id = (ImTextureID)tex->GetId();
			w = (float)tex->GetWidth();
			h = (float)tex->GetHeight();
		}
		else {
			im_id = 0;
			w = 1.f;
			h = 1.f;
		}

		ImGui::Image(im_id, ImGui::GetContentRegionAvail(), ImVec2((float)data.x / w, (float)data.y / h), ImVec2((float)(data.x + data.width) / w, (float)(data.y + data.height) / h));

		ImGui::EndChild();
	}

	string buffTemp = buff;
	ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll;
	vector<string> names = vector<string>();
	names.push_back("");

	for (auto item : GetAllSpritesNames()) if (item.second != _spriteAliases[spriteToEdit]) names.push_back(item.second);

	static bool error_name = std::find(names.begin(), names.end(), buffTemp) != names.end();

	if (ImGui::InputText("Name##SPRITE_EDITOR Sprite Manager", &buffTemp, flags)) {
		if (buffTemp != buff) {
			if (buffTemp.size() == 0 || std::find(names.begin(), names.end(), buffTemp) != names.end()) {
				error_name = true;
			}
			else {
				error_name = false;
			}
			buff = buffTemp;
		}
	}

	names.clear();

	if (error_name) ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Incorect Name or Name already exist!");

	map<size_t, string> textures = TextureManager::GetAllTexture2DNames();

	vector<std::pair<size_t, string>> texs = vector<std::pair<size_t, string>>(textures.begin(), textures.end());

	textures.clear();

	vector<string> texNames = vector<string>();
	vector<size_t> ids = vector<size_t>();
	texNames.resize(texs.size());
	ids.resize(texs.size());

	std::sort(texs.begin(), texs.end(), [&](std::pair<size_t, string> const& left, std::pair<size_t, string> const& right) -> bool {
		return left.second.compare(right.second) < 0;
		});

	std::transform(texs.begin(), texs.end(), texNames.begin(), [](std::pair<size_t, string> const& i) -> string {
		return i.second + "##SPRITE_EDITOR Sprite Manager" + std::to_string(i.first);
		});

	std::transform(texs.begin(), texs.end(), ids.begin(), [](std::pair<size_t, string> const& i) -> size_t {
		return i.first;
		});

	texs.clear();

	int choosed = -1;

	if (selectedTexture != 0) choosed = std::find(ids.begin(), ids.end(), selectedTexture) - ids.begin();

	if (ImGui::ComboWithFilter(string("Texture##SPRITE_EDITOR Sprite Manager").c_str(), &choosed, texNames, 10)) {
		if (choosed != -1) {
			tex = TextureManager::GetTexture2D(ids[choosed]);
			selectedTexture = ids[choosed];

			if (tex != nullptr) {
				if (data.x > tex->GetWidth()) data.x = 0;
				if (data.y > tex->GetHeight()) data.y = 0;
				if (data.x + data.width > tex->GetWidth() || data.width == 1) data.width = tex->GetWidth() - data.x;
				if (data.y + data.height > tex->GetHeight() || data.height == 1) data.height = tex->GetHeight() - data.y;
			}
			else {
				data.x = 0;
				data.y = 0;
				data.width = 1;
				data.height = 1;
			}
		}
	}

	texNames.clear();
	ids.clear();

	/*
	bool clicked = false;

	if (ImGui::BeginCombo(string("Texture##SPRITE_EDITOR Sprite Manager").c_str(), selectedTexture == 0 ? "None" : textures[selectedTexture].c_str())) {

		for (auto& item : textures)
		{
			size_t i = 0;
			if (ImGui::Selectable(std::string(item.second).append("##SPRITE_EDITOR Sprite Manager").append(std::to_string(i)).c_str(), selectedTexture == item.first))
			{
				if (clicked) {
					++i;
					continue;
				}
				selectedTexture = item.first;
				clicked = true;

				tex = TextureManager::GetTexture2D(selectedTexture);

				if (tex != nullptr) {
					if (data.x > tex->GetWidth()) data.x = 0;
					if (data.y > tex->GetHeight()) data.y = 0;
					if (data.x + data.width > tex->GetWidth() || data.width == 1) data.width = tex->GetWidth() - data.x;
					if (data.y + data.height > tex->GetHeight() || data.height == 1) data.height = tex->GetHeight() - data.y;
				}
				else {
					data.x = 0;
					data.y = 0;
					data.width = 1;
					data.height = 1;
				}
			}
			++i;
		}
		ImGui::EndCombo();
	}
	*/

	ImGui::BeginDisabled(selectedTexture == 0 || tex == nullptr);
	ImGui::DragUInt("X##SPRITE_EDITOR Sprite Manager", &data.x, 1.f, 0, tex != nullptr ? tex->GetWidth() - 1 : 0);
	ImGui::DragUInt("Y##SPRITE_EDITOR Sprite Manager", &data.y, 1.f, 0, tex != nullptr ? tex->GetHeight() - 1 : 0);
	ImGui::EndDisabled();

	if (tex != nullptr) {
		if (data.x + data.width > tex->GetWidth()) data.width = tex->GetWidth() - data.x;
		if (data.y + data.height > tex->GetHeight()) data.height = tex->GetHeight() - data.y;
	}

	ImGui::BeginDisabled(selectedTexture == 0 || tex == nullptr || (tex != nullptr ? tex->GetWidth() - data.x == 1 : false));
	ImGui::DragUInt("Width##SPRITE_EDITOR Sprite Manager", &data.width, 1.f, 1, tex != nullptr ? tex->GetWidth() - data.x : 1);
	ImGui::EndDisabled();

	ImGui::BeginDisabled(selectedTexture == 0 || tex == nullptr || (tex != nullptr ? tex->GetHeight() - data.y == 1 : false));
	ImGui::DragUInt("Height##SPRITE_EDITOR Sprite Manager", &data.height, 1.f, 1, tex != nullptr ? tex->GetHeight() - data.y : 1);
	ImGui::EndDisabled();

	ImGui::BeginDisabled(error_name || selectedTexture == 0 || data.width == 0 || data.height == 0);
	if (ImGui::Button("Save##SPRITE_EDITOR Sprite Manager", ImVec2(ImGui::GetContentRegionAvail().x, 0.f))) {
		EditSprite(spriteToEdit, selectedTexture, data, buff == _spriteAliases[spriteToEdit] ? "" : buff);
		*p_open = false;
	}
	ImGui::EndDisabled();

	ImGui::End();
}
#endif

Sprite* SpriteManager::MakeSprite(const string& spriteAlias, const string& texPath)
{
    return MakeSprite(spriteAlias, TextureManager::LoadTexture2D(texPath));
}

Sprite* SpriteManager::MakeSprite(const string& spriteAlias, Texture2D* tex)
{
    return MakeSprite(spriteAlias, tex, { .x = 0, .y = 0, .width = tex->GetWidth(), .height = tex->GetHeight() });
}

Sprite* SpriteManager::MakeSprite(const string& spriteAlias, size_t texManagerId)
{
    return MakeSprite(spriteAlias, TextureManager::GetTexture2D(texManagerId));
}

Sprite* SpriteManager::MakeSprite(const string& spriteAlias, const string& texPath, const SpriteData& data)
{
    return MakeSprite(spriteAlias, TextureManager::LoadTexture2D(texPath), data);
}

Sprite* SpriteManager::MakeSprite(const string& spriteAlias, Texture2D* tex, const SpriteData& data)
{
    size_t aliasHash = _hasher(spriteAlias);
    if (_sprites.contains(aliasHash)) {
        SPDLOG_WARN("Sprite '{0}' already created", aliasHash);
        return _sprites[aliasHash];
    }
    Sprite* spr = new Sprite(aliasHash, tex, data.x, data.y, data.width, data.height);
    _sprites[aliasHash] = spr;
    _spriteAliases[aliasHash] = spriteAlias;

    return spr;
}

Sprite* SpriteManager::MakeSprite(const string& spriteAlias, size_t texManagerId, const SpriteData& data)
{
    return MakeSprite(spriteAlias, TextureManager::GetTexture2D(texManagerId), data);
}

Sprite* SpriteManager::EditSprite(size_t spriteId, const std::string& texPath, const SpriteData& data, const std::string& newAlias)
{
	return EditSprite(spriteId, TextureManager::GetTexture2D(texPath), data, newAlias);
}

Sprite* SpriteManager::EditSprite(size_t spriteId, Texture2D* tex, const SpriteData& data, const std::string& newAlias)
{
	if (!_sprites.contains(spriteId)) {
		SPDLOG_WARN("Sprite '{0}' wasn't created", spriteId);
		return nullptr;
	}

	Sprite* spr = _sprites[spriteId];
	spr->_tex = tex;
	spr->_offset.x = data.x;
	spr->_offset.y = data.y;
	spr->_size.x = data.width;
	spr->_size.y = data.height;

	if (newAlias != "") {
		_sprites.erase(spriteId);
		_spriteAliases.erase(spriteId);

		_sprites[_hasher(newAlias)] = spr;
		_spriteAliases[_hasher(newAlias)] = newAlias;
	}

	return spr;
}

Sprite* SpriteManager::EditSprite(size_t spriteId, size_t texManagerId, const SpriteData& data, const std::string& newAlias)
{
	return EditSprite(spriteId, TextureManager::GetTexture2D(texManagerId), data, newAlias);
}

Sprite* SpriteManager::EditSprite(const std::string& spriteAlias, const std::string& texPath, const SpriteData& data, const std::string& newAlias)
{
	return EditSprite(_hasher(spriteAlias), TextureManager::GetTexture2D(texPath), data, newAlias);
}

Sprite* SpriteManager::EditSprite(const std::string& spriteAlias, Texture2D* tex, const SpriteData& data, const std::string& newAlias)
{
	return EditSprite(_hasher(spriteAlias), tex, data, newAlias);
}

Sprite* SpriteManager::EditSprite(const std::string& spriteAlias, size_t texManagerId, const SpriteData& data, const std::string& newAlias)
{
	return EditSprite(_hasher(spriteAlias), TextureManager::GetTexture2D(texManagerId), data, newAlias);
}

Sprite* SpriteManager::GetSprite(size_t spriteId)
{
    if (_sprites.contains(spriteId)) {
        return _sprites[spriteId];
    }
    return nullptr;
}

Sprite* SpriteManager::GetSprite(const string& spriteAlias)
{
    return GetSprite(_hasher(spriteAlias));
}

size_t SpriteManager::GetSpriteId(const std::string& spriteAlias)
{
	size_t id = _hasher(spriteAlias);

	if (!_spriteAliases.contains(id)) return 0;
	return id;
}

std::string SpriteManager::GetSpriteName(size_t spriteId)
{
	if (!_spriteAliases.contains(spriteId)) return "";
	return _spriteAliases[spriteId];
}

std::map<size_t, std::string> SpriteManager::GetAllSpritesNames() 
{
	return _spriteAliases;
}

void SpriteManager::UnloadAll()
{
    for (auto& spr : _sprites) {
        delete spr.second;
    }
    _sprites.clear();
    _spriteAliases.clear();
}

YAML::Node SpriteManager::Serialize()
{
    YAML::Node sprites;
    size_t id = 0;
    for (const auto& spritePair : SpriteManager::_spriteAliases) {
        Sprite* sprite = SpriteManager::_sprites[spritePair.first];

        YAML::Node spriteNode;
        spriteNode["id"] = id++;
        spriteNode["alias"] = spritePair.second;
        spriteNode["texture"] = sprite->GetTexture()->GetManagerId();
		spriteNode["x"] = sprite->GetXOffset();
		spriteNode["y"] = sprite->GetYOffset();
		spriteNode["width"] = sprite->GetWidth();
		spriteNode["height"] = sprite->GetHeight();

        sprites.push_back(spriteNode);
    }
    return sprites;
}

#if _DEBUG
void SpriteManager::DrawEditor(bool* p_open)
{
    if (!ImGui::Begin("Sprite Manager", p_open, ImGuiWindowFlags_MenuBar)) {
        ImGui::End();
        return;
    }

	static bool openCreator = false;

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File##Sprite Manager"))
		{
			if (ImGui::MenuItem("Create##Sprite Manager")) {
				openCreator = true;
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (openCreator) DrawSpriteCreator(&openCreator);

	ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	bool node_open = ImGui::TreeNodeEx(string("Sprites##Sprite Manager").c_str(), node_flag);

	std::list<size_t> clicked = std::list<size_t>();
	static size_t selectedToEdit = 0;
	static bool openEditor = true;
	clicked.clear();
	if (node_open) {
		int i = 0;
		for (auto& item : _spriteAliases) {
			string n = item.second;
			ImGui::BulletText(n.c_str());
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 35);
			if (ImGui::Button(string(ICON_FA_PENCIL "##Edit Sprite Manager").append(std::to_string(i)).c_str())) {
				selectedToEdit = item.first;
				openEditor = true;
				_editNext = true;
			}
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 10);
			if (ImGui::Button(string(ICON_FA_TRASH_CAN "##Remove Sprite Manager").append(std::to_string(i)).c_str())) {
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
			UnloadSprite(clicked.back());

			clicked.pop_back();
		}
	}

	if (selectedToEdit != 0) {
		DrawSpriteEditor(&openEditor, selectedToEdit);

		if (!openEditor) {
			selectedToEdit = 0;
		}
	}

	clicked.clear();

	ImGui::End();
}
#endif