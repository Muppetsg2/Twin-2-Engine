#pragma once

#if _DEBUG
#include <manager/ScriptableObjectManager.h>

namespace Editor::Common
{
	class ScriptableObjectEditorManager
	{
		static std::vector<std::string> _paths;

		struct ScriptableObjectRecord
		{
			size_t id;
			std::string name;
			std::string fullName;
			std::string path;
			std::vector<size_t> categoriesHashes;
		};

		struct ScriptableObjectRecordNode
		{
			std::string nodeName;

			std::unordered_map<size_t, ScriptableObjectRecord*> scriptableObjects;

			std::unordered_map<size_t, ScriptableObjectRecordNode*> subcateogries;
		};

		static ScriptableObjectRecordNode* _root;

		static std::unordered_map<size_t, ScriptableObjectRecord*> _trackedStrciptableObjects;

		static void AddScriptableObject(const std::filesystem::path& path);
		static void DrawNode(ScriptableObjectRecordNode* node, const ImGuiTreeNodeFlags& baseFlags, std::string& selectedSOName, std::string& selectedScriptableObjectPath, Twin2Engine::Core::ScriptableObject*& selectedScriptableObject);
		static void DeleteTree(ScriptableObjectRecordNode* node);

	public:

		static void Init();
		static void Update();
		static void End();
		static void Draw();
	};
}
#endif