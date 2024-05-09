#include <processes/TimerProcess.h>
#include <processes/ProcessManager.h>
#include <functional>


void TimerLoop(Twin2Engine::Processes::TimerProcess* timerProc)
{
	while (timerProc->work) {
		timerProc->Update();

		if (timerProc->work) {
			std::this_thread::sleep_until(timerProc->repeatTimePoint);
			timerProc->repeatTimePoint += std::chrono::milliseconds(timerProc->_repeatTimeMillis);
		}
	}
}

Twin2Engine::Processes::TimerProcess::~TimerProcess() {
	if (work) {
		work = false;
		t.join();
	}
	Twin2Engine::Processes::ProcessManager::Instance()->RemoveProcess(this);
}

void Twin2Engine::Processes::TimerProcess::Initialize()
{
	Twin2Engine::Processes::ProcessManager::Instance()->AddProcess(this);
}

void Twin2Engine::Processes::TimerProcess::Update()
{
	UpdateAction();

	//UpdateChildrenProcesses();
}

void Twin2Engine::Processes::TimerProcess::OnActivation()
{
	if (stopped) {
		Begin();
	}
}

void Twin2Engine::Processes::TimerProcess::OnDeactivation()
{
	if (!stopped) {
		Finish(true);
	}
}

bool Twin2Engine::Processes::TimerProcess::Begin()
{
	if (work) {
		return false;
	}
	work = true;
	stopped = false;

	repeatTimePoint = std::chrono::steady_clock::now() + std::chrono::milliseconds(_repeatTimeMillis);
	t = std::thread(TimerLoop, this);

	return true;
}

void Twin2Engine::Processes::TimerProcess::Finish(bool waitForFinish)
{
	if (work) {
		work = false;
		stopped = true;
		if (waitForFinish) {
			t.join();
		}
	}
}

void Twin2Engine::Processes::TimerProcess::SetRepeatTime(int repeatTimeMillis)
{
	_repeatTimeMillis = repeatTimeMillis;
}

YAML::Node Twin2Engine::Processes::TimerProcess::Serialize() const
{
	return std::move(Twin2Engine::Processes::Process::Serialize());
}