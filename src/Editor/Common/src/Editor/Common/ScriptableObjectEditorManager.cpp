#include <Editor/Common/ScriptableObjectEditorManager.h>

using namespace Editor::Common;
using namespace std;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

namespace fs = std::filesystem;


#define SOURCE_PATH  "res/scriptableobjects"
vector<string> ScriptableObjectEditorManager::_paths;

ScriptableObjectEditorManager::ScriptableObjectRecordNode* ScriptableObjectEditorManager::_root;
unordered_map<size_t, ScriptableObjectEditorManager::ScriptableObjectRecord*> ScriptableObjectEditorManager::_trackedStrciptableObjects;

void ScriptableObjectEditorManager::AddScriptableObject(const fs::path& path)
{
    hash<string> hasher;

    fs::path filePath;
    for (auto pathElement = ++++path.begin(); pathElement != path.end(); pathElement++)
    {
        filePath = filePath / *pathElement;
    }
    SPDLOG_INFO("ScriptableObject: {}", path.string());
    SPDLOG_INFO("ScriptableObject: {}", filePath.string());

    vector<string> categoriesNames;
    vector<size_t> categoriesHashes;
    size_t id = hasher(path.string());
    for (auto pathElement = filePath.begin(); pathElement != (--filePath.end()); pathElement++)
    {
        categoriesNames.push_back(pathElement->string());
        categoriesHashes.push_back(hasher(pathElement->string()));
    }
    
    ScriptableObjectRecordNode* currentNode = _root;

    for (size_t index = 0; index < categoriesHashes.size(); index++)
    {
        if (!currentNode->subcateogries.contains(categoriesHashes[index]))
        {
            currentNode->subcateogries[categoriesHashes[index]] = new ScriptableObjectRecordNode{
                .nodeName = categoriesNames[index]
            };
        }

        currentNode = currentNode->subcateogries[categoriesHashes[index]];
    }

    currentNode->scriptableObjects[id] = new ScriptableObjectRecord{
        .id = hasher(path.string()),
        .name = path.filename().string(),
        .fullName = filePath.string(),
        .path = path.string(),
        .categoriesHashes = categoriesHashes
    };
    
    _trackedStrciptableObjects[id] = currentNode->scriptableObjects[id];
    /*
    ScriptableObjectRecord* record = new ScriptableObjectRecord();
    record->name = path.filename().string();
    record->fullName = filePath.string();
    record->path = path.string();
    record->id = hasher(record->path);

    vector<string> categoriesNames;
    for (auto pathElement = filePath.begin(); pathElement != (--filePath.end()); pathElement++)
    {
        categoriesNames.push_back(pathElement->string());
        record->categoriesHashes.push_back(hasher(pathElement->string()));
    }
    _trackedStrciptableObjects[record->id] = record;

    ScriptableObjectRecordNode* currentNode = _root;

    for (size_t index = 0; index < record->categoriesHashes.size(); index++)
    {
        if (currentNode->subcateogries.contains(record->categoriesHashes[index]))
        {
            currentNode = currentNode->subcateogries[record->categoriesHashes[index]];
        }
        else
        {
            ScriptableObjectRecordNode* newNode = new ScriptableObjectRecordNode();

            newNode->nodeName = categoriesNames[index];

            currentNode->subcateogries[record->categoriesHashes[index]] = newNode;

            currentNode = newNode;
        }
    }

    currentNode->scriptableObjects[record->id] = record;
    */
}
void ScriptableObjectEditorManager::Update()
{
    hash<string> hasher;
    for (auto& entry : fs::recursive_directory_iterator(SOURCE_PATH)) 
    {
        if (entry.is_regular_file() && entry.path().extension() == ".so") 
        {
            AddScriptableObject(entry.path());
            /*fs::path filePath;
            for (auto pathElement = ++++entry.path().begin(); pathElement != entry.path().end(); pathElement++)
            {
                filePath = filePath / *pathElement;
            }
            SPDLOG_INFO("ScriptableObject: {}", entry.path().string());
            SPDLOG_INFO("ScriptableObject: {}", filePath.string());

            ScriptableObjectRecord* record = new ScriptableObjectRecord();
            record->name = entry.path().filename().string();
            record->fullName = filePath.string();
            record->path = entry.path().string();
            record->id = hasher(record->path);
            
            vector<string> categoriesNames;
            for (auto pathElement = filePath.begin(); pathElement != (--filePath.end()); pathElement++)
            {
                categoriesNames.push_back(pathElement->string());
                record->categoriesHashes.push_back(hasher(pathElement->string()));
            }
            _trackedStrciptableObjects[record->id] = record;

            ScriptableObjectRecordNode* currentNode = _root;

            for (size_t index = 0; index < record->categoriesHashes.size(); index++)
            {
                if (currentNode->subcateogries.contains(record->categoriesHashes[index]))
                {
                    currentNode = currentNode->subcateogries[record->categoriesHashes[index]];
                }
                else
                {
                    ScriptableObjectRecordNode* newNode = new ScriptableObjectRecordNode();

                    newNode->nodeName = categoriesNames[index];

                    currentNode->subcateogries[record->categoriesHashes[index]] = newNode;

                    currentNode = newNode;
                }
            }

            currentNode->scriptableObjects[record->id] = record;*/
        }
    }
}
void ScriptableObjectEditorManager::Init()
{
    _root = new ScriptableObjectRecordNode();
    _root->nodeName = "";
}

void ScriptableObjectEditorManager::End()
{
    for (auto& pair : _trackedStrciptableObjects)
    {
        pair.second->categoriesHashes.clear();
        delete pair.second;
    }

    DeleteTree(_root);
}

void ScriptableObjectEditorManager::DeleteTree(ScriptableObjectRecordNode* node)
{
    for (auto& pair : node->subcateogries)
    {
        DeleteTree(pair.second);
    }

    for (auto& item : node->scriptableObjects) {
        item.second = nullptr;
    }

    node->scriptableObjects.clear();

    delete node;
}

void ScriptableObjectEditorManager::Draw()
{
    if (ImGui::CollapsingHeader("Scriptable Object Manager"))
    {

        if (ImGui::CollapsingHeader("Scriptable Object Creator"))
        {
            static string selectedSO = "";
            static vector<string> scriptableObjectsNames = ScriptableObjectManager::GetScriptableObjectsNames();
            if (ImGui::TreeNode("ScriptableObjects")) {
                for (size_t i = 0; i < scriptableObjectsNames.size(); i++)
                {
                    if (ImGui::Selectable(scriptableObjectsNames[i].c_str())) {
                        selectedSO = scriptableObjectsNames[i];
                    }
                }
                ImGui::TreePop();
            }
            ImGui::Text("Selected Scriptable Object to create:");
            ImGui::SameLine();
            ImGui::InputText("##selectedItem", &selectedSO[0], selectedSO.size(), ImGuiInputTextFlags_ReadOnly);
            static char dstPath[255] = { '\0' };
            ImGui::Text("Destination and output file name:");
            ImGui::SameLine();
            ImGui::InputText("##dstPath", dstPath, 254);

            if (ImGui::Button("Create ScriptableObject"))
            {
                if (selectedSO.size() > 0)
                {
                    string strDstPath(dstPath);
                    if (strDstPath.size() > 0)
                    {
                        ScriptableObjectManager::CreateScriptableObject(strDstPath, selectedSO);
                        AddScriptableObject(strDstPath);
                    }
                }
            }
        }

        if (ImGui::CollapsingHeader("Scriptable Object Modifier"))
        {
            static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;


            static string selectedSOName = "";
            static string selectedScriptableObjectPath = "";
            static ScriptableObject* selectedScriptableObject = nullptr;

            ImGuiTreeNodeFlags nodeFlags = base_flags;// | ImGuiTreeNodeFlags_Framed;
            if (!_root->subcateogries.size() && !_root->scriptableObjects.size())
            {
                nodeFlags |= ImGuiTreeNodeFlags_Leaf;
            }
            bool node_open = ImGui::TreeNodeEx((void*)_root, nodeFlags, "Scriptable Objects List");

            if (node_open)
            {
                for (auto& pair : _root->scriptableObjects)
                {
                    if (ImGui::Selectable(pair.second->name.c_str()))
                    {
                        selectedSOName = pair.second->fullName;
                        selectedScriptableObjectPath = pair.second->path;
                        selectedScriptableObject = Twin2Engine::Manager::ScriptableObjectManager::Get(pair.second->path);
                        if (!selectedScriptableObject)
                        {
                            selectedScriptableObject = Twin2Engine::Manager::ScriptableObjectManager::Load(pair.second->path);
                        }
                    }
                }
                //ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
                for (auto& pair : _root->subcateogries)
                {
                    DrawNode(pair.second, base_flags, selectedSOName, selectedScriptableObjectPath, selectedScriptableObject);
                }
                ImGui::TreePop();
            }

            ImGui::Text("Selected Scriptable Object to modify:");
            ImGui::InputText("##selectedSO", &selectedSOName[0], selectedSOName.size(), ImGuiInputTextFlags_ReadOnly);

            ImGui::Separator();
            if (selectedScriptableObject != nullptr)
            {
                selectedScriptableObject->DrawEditor();
            }
            ImGui::Separator();
            if (ImGui::Button("Save"))
            {
                if (selectedScriptableObject != nullptr)
                {
                    ScriptableObjectManager::Save(selectedScriptableObjectPath, selectedScriptableObject);
                }
            }
            ImGui::Separator();
        }
    }
}

void ScriptableObjectEditorManager::DrawNode(ScriptableObjectRecordNode* node, const ImGuiTreeNodeFlags& baseFlags, string& selectedSOName, std::string& selectedScriptableObjectPath, ScriptableObject*& selectedScriptableObject)
{
    ImGuiTreeNodeFlags nodeFlags = baseFlags;
    if (!node->subcateogries.size() && !node->scriptableObjects.size())
    {
        nodeFlags |= ImGuiTreeNodeFlags_Leaf;
    }
    bool node_open = ImGui::TreeNodeEx((void*)node, nodeFlags, node->nodeName.c_str());


    if (node_open)
    {
        for (auto& pair : node->scriptableObjects)
        {
            if (ImGui::Selectable(pair.second->name.c_str())) 
            {
                selectedSOName = pair.second->fullName;
                selectedScriptableObjectPath = pair.second->path;
                selectedScriptableObject = Twin2Engine::Manager::ScriptableObjectManager::Get(pair.second->path);
                if (!selectedScriptableObject)
                {
                    selectedScriptableObject = Twin2Engine::Manager::ScriptableObjectManager::Load(pair.second->path);
                }
            }
        }

        //ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
        for (auto& pair : node->subcateogries)
        {
            DrawNode(pair.second, baseFlags, selectedSOName, selectedScriptableObjectPath, selectedScriptableObject);
        }
        ImGui::TreePop();
    }
}