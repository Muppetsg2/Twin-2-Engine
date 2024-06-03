#include <manager/ScriptableObjectManager.h>
#include <core/ScriptableObject.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

using namespace std;

hash<std::string> ScriptableObjectManager::_hasher;

#if _DEBUG
bool ScriptableObjectManager::_fileDialogOpen = false;
ImFileDialogInfo ScriptableObjectManager::_fileDialogInfo;
#endif

unordered_map<size_t, ScriptableObject*> ScriptableObjectManager::_scriptableObjects;

unordered_map<size_t, string> ScriptableObjectManager::_scriptableObjectsPaths;

unordered_map<size_t, unsigned int> ScriptableObjectManager::_serializationContext;
unordered_map<unsigned int, size_t> ScriptableObjectManager::_deserializationContext;

ScriptableObject* ScriptableObjectManager::Load(const std::string& path)
{
	size_t pathHash = _hasher(path);
	if (_scriptableObjects.contains(pathHash)) {
		SPDLOG_WARN("ScriptableObject '{0}' already loaded", path);
		return _scriptableObjects[pathHash];
	}

	SPDLOG_INFO("Loading ScriptableObject '{0}'", path);

	if (filesystem::exists(path))
	{
		YAML::Node soNode = YAML::LoadFile(path)["scriptable_object"];
		size_t hashedSORegisteredName = _hasher(soNode["__SO_RegisteredName__"].as<string>());
		ScriptableObject* scriptableObject = ScriptableObject::scriptableObjects[hashedSORegisteredName].createSpecificScriptableObject();
		scriptableObject->_id = pathHash;

		scriptableObject->Deserialize(soNode);

		_scriptableObjects[pathHash] = scriptableObject;
		_scriptableObjectsPaths[pathHash] = path;

		return scriptableObject;
	}
	else
	{
		SPDLOG_ERROR("Scriptable Object file '{0}' not found!", path);
		return nullptr;
	}
}

void ScriptableObjectManager::Unload(const std::string& path) 
{
	Unload(_hasher(path));
}

void ScriptableObjectManager::Unload(size_t id)
{
	if (!_scriptableObjects.contains(id)) {
		SPDLOG_WARN("Scriptable Object file '{0}' not found!", _scriptableObjectsPaths[id]);
		return;
	}

	delete _scriptableObjects[id];

	_scriptableObjects.erase(id);
	_scriptableObjectsPaths.erase(id);
}

ScriptableObject* ScriptableObjectManager::Get(const std::string& path)
{
	size_t pathHash = _hasher(path);
	if (_scriptableObjects.contains(pathHash)) {
		//SPDLOG_WARN("ScriptableObject '{0}' already loaded", path);
		return _scriptableObjects[pathHash];
	}
	SPDLOG_WARN("ScriptableObject '{0}' not loaded", path);
	return nullptr;
}

ScriptableObject* ScriptableObjectManager::Get(size_t id)
{
	if (_scriptableObjects.contains(id))
	{
		return _scriptableObjects[id];
	}
	return nullptr;
}

std::string ScriptableObjectManager::GetPath(size_t id)
{
	if (_scriptableObjects.contains(id))
	{
		return _scriptableObjectsPaths[id];
	}
	SPDLOG_WARN("ScriptableObject '{0}' not loaded", id);
	return "";
}

std::string ScriptableObjectManager::GetName(const std::string& path)
{
	return GetName(_hasher(path));
}

std::string ScriptableObjectManager::GetName(size_t id)
{
	if (!_scriptableObjectsPaths.contains(id)) return "";
	string p = _scriptableObjectsPaths[id];
	return std::filesystem::path(p).stem().string();
}

void ScriptableObjectManager::UnloadAll()
{
	for (auto& pair : _scriptableObjects)
	{
		delete pair.second;
	}
	_scriptableObjects.clear();
	_scriptableObjectsPaths.clear();
}

YAML::Node ScriptableObjectManager::Serialize()
{
	YAML::Node node;

	for (const auto& pair : _serializationContext)
	{
		node[pair.second]["path"] = _scriptableObjectsPaths[pair.first];
	}
	return node;
}

void ScriptableObjectManager::SceneSerializationBegin()
{
	_serializationContext.clear();
}

void ScriptableObjectManager::SceneSerializationEnd()
{
	_serializationContext.clear();
}

unsigned int ScriptableObjectManager::SceneSerialize(size_t id)
{
	if (_serializationContext.contains(id))
	{
		return _serializationContext[id];
	}
	unsigned int index = _serializationContext.size();
	_serializationContext[id] = index;
	return index;
}

void ScriptableObjectManager::SceneDeserializationBegin()
{
	_deserializationContext.clear();
}

size_t ScriptableObjectManager::SceneDeserialize(unsigned int sceneSerializationId, const std::string& path)
{
	if (_deserializationContext.contains(sceneSerializationId))
	{
		return _deserializationContext[sceneSerializationId];
	}
	size_t hashedPath = _hasher(path);
	_deserializationContext[sceneSerializationId] = hashedPath;
	return hashedPath;
}

ScriptableObject* ScriptableObjectManager::Deserialize(unsigned int sceneSerializationId)
{
	if (_deserializationContext.contains(sceneSerializationId))
	{
		return _scriptableObjects[_deserializationContext[sceneSerializationId]];
	}
	return nullptr;
}

#if _DEBUG
void ScriptableObjectManager::DrawEditor(bool* p_open) {
	if (!ImGui::Begin("Scriptable Object Manager", p_open, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}

	static bool openCreator = false;

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File##Scriptable Object Manager"))
		{
			if (ImGui::MenuItem("Create##Scriptable Object Manager")) {
				openCreator = true;
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (openCreator) DrawCreator(&openCreator);

	ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	bool node_open = ImGui::TreeNodeEx(string("Scriptable Objects##Scriptable Object Manager").c_str(), node_flag);

	std::list<size_t> clicked = std::list<size_t>();
	static size_t selectedToEdit = 0;
	static bool openEditor = true;
	clicked.clear();
	if (node_open) {
		int i = 0;
		for (auto& item : _scriptableObjectsPaths) {
			string n = GetName(item.second);
			ImGui::BulletText(n.c_str());
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70);
			if (ImGui::Button(string("Edit##Scriptable Object Manager").append(std::to_string(i)).c_str())) {
				selectedToEdit = item.first;
				openEditor = true;
			}
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 10);
			if (ImGui::Button(string("##Remove Scriptable Object Manager").append(std::to_string(i)).c_str())) {
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

	if (selectedToEdit != 0) {
		if (ImGui::Begin("Editor##Scriptable Object Manager", &openEditor)) {
			_scriptableObjects[selectedToEdit]->DrawEditor();
		}
		ImGui::End();

		if (!openEditor) {
			selectedToEdit = 0;
		}
	}

	clicked.clear();

	if (ImGui::Button("Load Scriptable Object##Scriptable Object Manager", ImVec2(ImGui::GetContentRegionAvail().x, 0.f))) {
		_fileDialogOpen = true;
		_fileDialogInfo.type = ImGuiFileDialogType_OpenFile;
		_fileDialogInfo.title = "Open File##Scriptable Object Manager";
		_fileDialogInfo.directoryPath = std::filesystem::path(std::filesystem::current_path().string() + "\\res\\scriptableobjects");
	}

	if (ImGui::FileDialog(&_fileDialogOpen, &_fileDialogInfo))
	{
		// Result path in: m_fileDialogInfo.resultPath
		Load(_fileDialogInfo.resultPath.string());
	}

	ImGui::End();
}
#endif

void ScriptableObjectManager::SceneDeserializationEnd()
{
	_deserializationContext.clear();
}

#if _DEBUG
void ScriptableObjectManager::DrawCreator(bool* p_open) 
{
	if (!ImGui::Begin("Scriptable Object Creator", p_open)) {
		ImGui::End();
		return;
	}

	static string buff = "New Scriptable Object";
	string buffTemp = buff;
	ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll;
	vector<string> names = GetScriptableObjectsNames();
	static bool error = std::find(names.begin(), names.end(), buffTemp) != names.end();

	if (ImGui::InputText("Name##SO_CREATOR Scriptable Object Manager", &buffTemp, flags)) {
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
	
	vector<string> classNames = ScriptableObjectManager::GetScriptableObjectsClassNames();

	if (classNames.size() == 0) {
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "There is no class implementing Scriptable Object!!!");
		ImGui::PopFont();
		return;
	}

	static string selectedSO = classNames[0];
	bool clicked = false;

	if (ImGui::BeginCombo(string("Type##SO_CREATOR Scriptable Object Manager").c_str(), selectedSO.c_str())) {

		for (size_t i = 0; i < classNames.size(); ++i)
		{
			if (ImGui::Selectable(std::string(classNames[i]).append("##SO_CREATOR Scriptable Object Manager").c_str(), selectedSO == classNames[i]))
			{
				if (clicked) {
					continue;
				}
				selectedSO = classNames[i];
				clicked = true;
			}
		}
		ImGui::EndCombo();
	}

	static bool saveErr = false;

	if (ImGui::Button("Create##SO_CREATOR Scriptable Object Manager", ImVec2(ImGui::GetContentRegionAvail().x, 0.f))) {
		std::string path = filesystem::current_path().string().append("/res/scriptableobjects/").append(buff).append(".so");
		bool res = ScriptableObjectManager::CreateScriptableObject(path, selectedSO);

		if (res) {
			saveErr = false;
			*p_open = false;
			ScriptableObjectManager::Load(path);
		}
		else {
			saveErr = true;
		}
	}

	if (saveErr) ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), std::string("There was problem when creating ScriptableObject: '").append("/res/scriptableobjects/").append(buff).append(".so'").c_str());

	ImGui::End();
}
#endif

vector<string> ScriptableObjectManager::GetScriptableObjectsNames()
{
	std::vector<string> scriptableObjectNames = std::vector<string>();

	for (auto& pair : _scriptableObjectsPaths) {
		scriptableObjectNames.push_back(GetName(pair.first));
	}

	return scriptableObjectNames;
}

vector<string> ScriptableObjectManager::GetScriptableObjectsClassNames()
{
	vector<string> scriptableObjectClassNames(ScriptableObject::scriptableObjects.size());
	size_t index = 0;
	for (auto& pair : ScriptableObject::scriptableObjects)
	{
		scriptableObjectClassNames[index++] = pair.second.scriptableObjectName;
	}
	return scriptableObjectClassNames;
}

bool ScriptableObjectManager::CreateScriptableObject(const string& dstPath, const string& scriptableObjectClassName)
{
	size_t hashedName = _hasher(scriptableObjectClassName);

	SPDLOG_INFO("Creating ScriptableObject Id: {}, Name {}, DatPath {}", hashedName, scriptableObjectClassName, dstPath);

	ScriptableObject* createdSO = nullptr;

	if (ScriptableObject::scriptableObjects.contains(hashedName))
	{
		SPDLOG_INFO("Destinated ScriptableObject Class exist!");
		createdSO = ScriptableObject::scriptableObjects[hashedName].createSpecificScriptableObject();
	}
	else {
		SPDLOG_WARN("Destinated ScriptableObject Class doesn't exist!");
	}

	return Save(dstPath, createdSO);
}

bool ScriptableObjectManager::Save(const string& dstPath, ScriptableObject* scriptableObject)
{
	if (scriptableObject != nullptr)
	{
		YAML::Node node;
		YAML::Node soNode;
		scriptableObject->Serialize(soNode);
		node["scriptable_object"] = soNode;

		filesystem::create_directories(filesystem::path(dstPath).parent_path());

		SPDLOG_INFO("ScriptableObject Full Path: {}", filesystem::path(dstPath).parent_path().string().c_str());

		ofstream file{ dstPath };
		if (file.is_open())
		{
			file << node;
		}
		else
		{
			SPDLOG_ERROR("Couldn't open file: {}, for saving ScriptableObject!", dstPath);
			file.close();
			return false;
		}
		file.close();

		return true;
	}

	return false;
}

vector<string> ScriptableObjectManager::GetAllPaths()
{
	vector<string> paths;
	paths.reserve(_scriptableObjectsPaths.size());
	for (auto& pair : _scriptableObjectsPaths)
	{
		paths.push_back(pair.second);
	}
	return paths;
}