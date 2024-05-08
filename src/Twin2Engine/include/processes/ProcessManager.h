#ifndef _PROCESSMANAGER_H_
#define _PROCESSMANAGER_H_

#include <unordered_set>
#include <processes/SynchronizedProcess.h>
#include <processes/ThreadProcess.h>
#include <processes/TimerProcess.h>

namespace Twin2Engine::Processes {
	class ProcessManager {
		friend SynchronizedProcess;
		friend ThreadProcess;
		friend TimerProcess;
		private:
			static ProcessManager* instance;
			std::unordered_set<SynchronizedProcess*> synchronizedProcesses;
			std::unordered_set<Process*> otherProcesses;
			ProcessManager() {}

			void AddProcess(SynchronizedProcess* process);
			void AddProcess(ThreadProcess* process);
			void AddProcess(TimerProcess* process);
			void RemoveProcess(SynchronizedProcess* process);
			void RemoveProcess(ThreadProcess* process);
			void RemoveProcess(TimerProcess* process);
		public:
			static ProcessManager* Instance();
			static void DeleteInstance();
			virtual ~ProcessManager();

			void UpdateSynchronizedProcess();
			Process* GetProcess(unsigned short GPID);
			Process* GetProcess(std::string name);
			void ReloadProcesses();
	};
}

#endif // !_PROCESSMANAGER_H_
