#include <Editor/Common/ScriptableObjectEditorManager.h>

using namespace Editor::Common;
using namespace std;
//using namespace Twin2Engine::Core;
//using namespace Twin2Engine::Manager;

namespace fs = std::filesystem;


#define SOURCE_PATH  "res/scriptableobjects"
vector<string> ScriptableObjectEditorManager::_paths;

ScriptableObjectEditorManager::ScriptableObjectRecordNode* ScriptableObjectEditorManager::_root;
unordered_map<size_t, ScriptableObjectEditorManager::ScriptableObjectRecord*> ScriptableObjectEditorManager::_trackedStrciptableObjects;

void ScriptableObjectEditorManager::Update()
{
    hash<string> hasher;
    for (auto& entry : fs::recursive_directory_iterator(SOURCE_PATH)) {
        if (entry.is_regular_file() && entry.path().extension() == ".so") {
            fs::path filePath;
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
            for (auto pathElement = filePath.begin(); pathElement != filePath.end(); pathElement++)
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

    delete node;
}