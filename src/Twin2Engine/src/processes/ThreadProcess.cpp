#include <processes/ThreadProcess.h>
#include <processes/ProcessManager.h>

void ThreadLoop(Twin2Engine::Processes::ThreadProcess* threadProc)
{
	while (threadProc->work) {
		threadProc->Update();

		if (threadProc->goSleep) {
			std::this_thread::sleep_for(threadProc->sleepTime);
			threadProc->goSleep = false;
		}

		if (threadProc->work) {
			std::this_thread::sleep_for(threadProc->delayTime);
		}
	}
}

Twin2Engine::Processes::ThreadProcess::~ThreadProcess() {
	if (work) {
		work = false;
		t.join();
	}
	Twin2Engine::Processes::ProcessManager::Instance()->RemoveProcess(this);
}

void Twin2Engine::Processes::ThreadProcess::Initialize()
{
	Twin2Engine::Processes::ProcessManager::Instance()->AddProcess(this);
}

void Twin2Engine::Processes::ThreadProcess::Update()
{
	UpdateAction();

	//UpdateChildrenProcesses();
}

bool Twin2Engine::Processes::ThreadProcess::Begin()
{
	if (work) {
		return false;
	}
	work = true;
	stopped = false;
	//auto&& ptr = &ThreadLoop;
	t = std::thread(ThreadLoop, this);

	return true;
}

void Twin2Engine::Processes::ThreadProcess::Finish(bool waitForFinish)
{
	if (work) {
		work = false;
		stopped = true;
		if (waitForFinish) {
			t.join();
		}
	}
}

std::thread* Twin2Engine::Processes::ThreadProcess::getThread()
{
	return &t;
}

void Twin2Engine::Processes::ThreadProcess::GoSleep(float timeMillis)
{
	goSleep = true;
	sleepTime = std::chrono::duration<float, std::milli>(timeMillis);

}

void Twin2Engine::Processes::ThreadProcess::SetDelayTime(float timeMillis)
{
	delayTime = std::chrono::duration<float, std::milli>(timeMillis);
}

void Twin2Engine::Processes::ThreadProcess::OnActivation()
{
	if (stopped) {
		Begin();
	}
}

void Twin2Engine::Processes::ThreadProcess::OnDeactivation()
{
	if (!stopped) {
		Finish(true);
	}
}
