#pragma once


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

		static void DrawNode(ScriptableObjectRecordNode* node);
		static void DeleteTree(ScriptableObjectRecordNode* node);

	public:

		static void Init();
		static void Update();
		static void End();
		static void Draw();
	};
}