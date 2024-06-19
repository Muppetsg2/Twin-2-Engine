#include <core/Time.h>
#include <GLFW/glfw3.h>

using namespace Twin2Engine::Core;

float Time::_updateDeltaTime = 0.f;
float Time::_timeMultiplier = 1.0f;

void Time::Update()
{
	static float t1 = glfwGetTime();
	static float t2 = glfwGetTime();

	t1 = t2;
	t2 = glfwGetTime();
	_updateDeltaTime = (t2 - t1) * _timeMultiplier;
}

float Twin2Engine::Core::Time::GetTime()
{
	return glfwGetTime();
}

float Time::GetDeltaTime()
{
	return _updateDeltaTime;
}