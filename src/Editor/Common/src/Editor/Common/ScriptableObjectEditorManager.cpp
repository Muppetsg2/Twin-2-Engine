#include <Editor/Common/ScriptableObjectEditorManager.h>

using namespace Editor::Common;
using namespace std;
//using namespace Twin2Engine::Core;
//using namespace Twin2Engine::Manager;

namespace fs = std::filesystem;


#define SOURCE_PATH  "res/scriptableobjects"
vector<string> ScriptableObjectEditorManager::_paths;

void ScriptableObjectEditorManager::Update()
{
    for (const auto& entry : fs::recursive_directory_iterator(SOURCE_PATH)) {
        if (entry.is_regular_file() && entry.path().extension() == ".so") {
            SPDLOG_INFO("ScriptableObject: {}", entry.path().string());
            //soFiles.push_back(entry.path().string());
        }
    }
}
