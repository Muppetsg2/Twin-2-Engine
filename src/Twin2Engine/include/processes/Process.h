#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <unordered_set>
#include <string>
#include <core/YamlConverters.h>

namespace Twin2Engine::Processes {
	class SynchronizedProcess;

	class Process {
		private:
			static unsigned short lastGPID;
			//std::unordered_set<SynchronizedProcess*> childrenProcesses;
			//void SetHierachyActive(bool& v);
		protected:
			Process() {}
		public:
			const unsigned short GPID = ++lastGPID; //Game Process Identificator
			bool Active = true;
			bool DontDestroyOnLoad = false;
			std::string name = "";
			std::size_t nameHash = std::hash<std::string>{}("");
			//bool HierachyActive = true;
			//Process* parent = nullptr;
			virtual ~Process() = 0 {}

			void SetActive(bool v);
			bool IsActive();
			void SetName(std::string name);
			//
			//void AddChild(SynchronizedProcess* process);
			//void RemoveChild(SynchronizedProcess* process);
			//SynchronizedProcess* RemoveChild(unsigned short GPID);
			//void ClearChildrenList();

			virtual void Initialize();
			virtual void Update();
			virtual void OnActivation();
			virtual void OnDeactivation();
			//void UpdateChildrenProcesses();

			virtual YAML::Node Serialize() const;
	};
}

#endif // !_PROCESS_H_
