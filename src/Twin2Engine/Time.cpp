#include <core/Time.h>
#include <GLFW/glfw3.h>

using namespace Twin2EngineCore;

float Time::_updateDeltaTime = 0.f;

void Time::Update()
{
	static float t1 = glfwGetTime();
	static float t2 = glfwGetTime();

	t1 = t2;
	t2 = glfwGetTime();
	_updateDeltaTime = t2 - t1;
}

float Time::GetDeltaTime()
{
	return _updateDeltaTime;
}