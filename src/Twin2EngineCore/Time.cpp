#include <inc/Time.h>
#include <chrono>

using namespace Twin2EngineCore;
using namespace std::chrono;

float Time::_updateDeltaTime = 0.f;

void Time::Update()
{
	static float t1 = system_clock::now();
	static float t2 = system_clock::now();

	t1 = t2;
	t2 = system_clock::now();
	_updateDeltaTime = (t2 - t1).count();
}

float Time::GetDeltaTime()
{
	return _updateDeltaTime;
}