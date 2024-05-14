#pragma once


namespace Editor::Common
{
	class ScriptableObjectEditorManager
	{
		static std::vector<std::string> _paths;
		
		struct ScriptableObjectRecordNode
		{
			std::unordered_map<size_t, std::string> scriptableObjectsPaths;

			std::vector<ScriptableObjectRecordNode*> subcateogries;
			std::vector<size_t> subcateogriesHashes;
			std::vector<std::string> subcateogriesNames;
		};

		static ScriptableObjectRecordNode* _root;

	public:

		static void Update();
	};
}