#include <graphic/manager/SpriteManager.h>
#include <graphic/manager/TextureManager.h>

using namespace Twin2Engine;
using namespace Graphic;
using namespace Manager;
using namespace std;

hash<string> SpriteManager::_hasher;
map<size_t, Sprite*> SpriteManager::_sprites;

map<size_t, string> SpriteManager::_spriteAliases;
map<size_t, SpriteData> SpriteManager::_spriteLoadData;

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
void SpriteManager::DrawSpriteEditor(bool* p_open, size_t spriteToEdit) {
	if (!ImGui::Begin("Sprite Creator##Sprite Manager", p_open)) {
		ImGui::End();
		return;
	}

	static SpriteData data { 0, 0, 0, 0};
	static Texture2D* tex = nullptr;
	static ImTextureID im_id = 0;
	static float w = 1.f;
	static float h = 1.f;

	if (ImGui::BeginChild("Preview##Sprite Manager", ImVec2(100, 100))) {
		
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

	if (error) ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Incorect Name or Name already exist!");

	map<size_t, string> textures = TextureManager::GetAllTexture2DNames();
	textures[0] = "None";

	static size_t selectedTexture = 0;
	bool clicked = false;

	if (ImGui::BeginCombo(string("Texture##SPRITE_CREATOR Sprite Manager").c_str(), textures[selectedTexture].c_str())) {

		for (auto& item : textures)
		{
			if (ImGui::Selectable(std::string(item.second).append("##SPRITE_CREATOR Sprite Manager").c_str(), selectedTexture == item.first))
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
					if (data.x + data.width> tex->GetWidth() || data.width == 0) data.width = tex->GetWidth() - data.x;
					if (data.y + data.height > tex->GetHeight() || data.height == 0) data.height = tex->GetHeight() - data.y;
				}
				else {
					data.x = 0;
					data.y = 0;
					data.width = 0;
					data.height = 0;
				}
			}
		}
		ImGui::EndCombo();
	}

	ImGui::BeginDisabled(selectedTexture == 0 || tex == nullptr);
	ImGui::DragUInt("X##SPRITE_CREATOR Sprite Manager", &data.x, 1.f, 0, tex != nullptr ? tex->GetWidth() - 1 : 0);
	ImGui::DragUInt("Y##SPRITE_CREATOR Sprite Manager", &data.y, 1.f, 0, tex != nullptr ? tex->GetHeight() - 1 : 0);

	if (tex != nullptr) {
		if (data.x + data.width > tex->GetWidth()) data.width = tex->GetWidth() - data.x;
		if (data.y + data.height > tex->GetHeight()) data.height = tex->GetHeight() - data.y;
	}

	ImGui::DragUInt("Width##SPRITE_CREATOR Sprite Manager", &data.width, 1.f, 0, tex != nullptr ? tex->GetWidth() - data.x : 0);
	ImGui::DragUInt("Height##SPRITE_CREATOR Sprite Manager", &data.height, 1.f, 0, tex != nullptr ? tex->GetHeight() - data.y : 0);
	ImGui::EndDisabled();

	ImGui::BeginDisabled(error || selectedTexture == 0 || data.width == 0 || data.height == 0);
	if (ImGui::Button("Create##SPRITE_CREATOR Sprite Manager", ImVec2(ImGui::GetContentRegionAvail().x, 0.f))) {
		MakeSprite(buff, selectedTexture, data);
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

    if (data.x != 0 || data.y != 0 || data.width != tex->GetWidth() || data.height != tex->GetHeight()) {
        _spriteLoadData[aliasHash] = data;
    }

    return spr;
}

Sprite* SpriteManager::MakeSprite(const string& spriteAlias, size_t texManagerId, const SpriteData& data)
{
    return MakeSprite(spriteAlias, TextureManager::GetTexture2D(texManagerId), data);
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

std::map<size_t, std::string> SpriteManager::GetAllSpritesNames() {
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

        if (SpriteManager::_spriteLoadData.contains(spritePair.first)) {
            SpriteData data = SpriteManager::_spriteLoadData[spritePair.first];
            spriteNode["x"] = data.x;
            spriteNode["y"] = data.y;
            spriteNode["width"] = data.width;
            spriteNode["height"] = data.height;
        }

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

	if (openCreator) DrawSpriteEditor(&openCreator);

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
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70);
			if (ImGui::Button(string("Edit##Sprite Manager").append(std::to_string(i)).c_str())) {
				selectedToEdit = item.first;
				openEditor = true;
			}
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 10);
			if (ImGui::RemoveButton(string("##Remove Sprite Manager").append(std::to_string(i)).c_str())) {
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