#include <processes/ProcessManager.h>

static Twin2Engine::Processes::ProcessManager* instance;

void Twin2Engine::Processes::ProcessManager::UpdateSynchronizedProcess()
{
	for (auto p : synchronizedProcesses) {
		p->Update();
	}
}

Twin2Engine::Processes::Process* Twin2Engine::Processes::ProcessManager::GetProcess(unsigned short GPID) {
	for (auto p : synchronizedProcesses) {
		if (p->GPID == GPID) {
			return p;
		}
	}

	for (auto p : otherProcesses) {
		if (p->GPID == GPID) {
			return p;
		}
	}
}

Twin2Engine::Processes::Process* Twin2Engine::Processes::ProcessManager::GetProcess(std::string name) {
	size_t hash = std::hash<std::string>{}(name);
	for (auto p : synchronizedProcesses) {
		if (p->nameHash == hash) {
			return p;
		}
	}

	for (auto p : otherProcesses) {
		if (p->nameHash == hash) {
			return p;
		}
	}
}

void Twin2Engine::Processes::ProcessManager::ReloadProcesses()
{
	//std::unordered_set<SynchronizedProcess*> newSynchronizedProcesses;
	//std::unordered_set<Process*> newOtherProcesses;
	//
	//for (auto p : synchronizedProcesses) {
	//	if (p->DontDestroyOnLoad) {
	//		newSynchronizedProcesses.insert(p);
	//	}
	//}
	//
	//for (auto p : otherProcesses) {
	//	if (p->DontDestroyOnLoad) {
	//		newOtherProcesses.insert(p);
	//	}
	//}
	//
	//synchronizedProcesses = newSynchronizedProcesses;
	//otherProcesses = newOtherProcesses;
	if (synchronizedProcesses.size() > 0) {
		auto itr1 = --synchronizedProcesses.end();
		auto itr1B = synchronizedProcesses.begin();
		while (itr1 != itr1B) {
			if (!(*itr1)->DontDestroyOnLoad) {
				auto v = synchronizedProcesses.extract(itr1);
				delete v.value();
			}
			--itr1;
		}
	}
	
	if (otherProcesses.size() > 0) {
		auto itr2 = --otherProcesses.end();
		auto itr2B = otherProcesses.begin();
		while (itr2 != itr2B) {
			if (!(*itr2)->DontDestroyOnLoad) {
				auto v = otherProcesses.extract(itr2);
				delete v.value();
			}
			--itr2;
		}
	}
}

void Twin2Engine::Processes::ProcessManager::AddProcess(SynchronizedProcess* process)
{
	synchronizedProcesses.insert(process);
}

void Twin2Engine::Processes::ProcessManager::AddProcess(ThreadProcess* process)
{
	otherProcesses.insert(process);
}

void Twin2Engine::Processes::ProcessManager::AddProcess(TimerProcess* process)
{
	otherProcesses.insert(process);
}

void Twin2Engine::Processes::ProcessManager::RemoveProcess(SynchronizedProcess* process)
{
	synchronizedProcesses.erase(process);
}

void Twin2Engine::Processes::ProcessManager::RemoveProcess(ThreadProcess* process)
{
	otherProcesses.erase(process);
}

void Twin2Engine::Processes::ProcessManager::RemoveProcess(TimerProcess* process)
{
	otherProcesses.erase(process);
}

Twin2Engine::Processes::ProcessManager* Twin2Engine::Processes::ProcessManager::Instance()
{
	if (instance == nullptr) {
		instance = new ProcessManager();
	}
	return instance;
}

void Twin2Engine::Processes::ProcessManager::DeleteInstance()
{
	if (instance == nullptr) {
		delete instance;
		instance = nullptr;
	}
}

Twin2Engine::Processes::ProcessManager::~ProcessManager()
{
	while (synchronizedProcesses.size() > 0) {
		auto v = synchronizedProcesses.extract(synchronizedProcesses.begin());
		delete v.value();
	}

	while (otherProcesses.size() > 0) {
		auto v = otherProcesses.extract(otherProcesses.begin());
		delete v.value();
	}
}
