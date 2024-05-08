#include <processes/SynchronizedProcess.h>
#include <processes/ProcessManager.h>


void Twin2Engine::Processes::SynchronizedProcess::Initialize()
{
	Twin2Engine::Processes::ProcessManager::Instance()->AddProcess(this);
}

void Twin2Engine::Processes::SynchronizedProcess::Update() {
	UpdateAction();

	//UpdateChildrenProcesses();
}

Twin2Engine::Processes::SynchronizedProcess::~SynchronizedProcess() {
	Twin2Engine::Processes::ProcessManager::Instance()->RemoveProcess(this);
}