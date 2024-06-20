#pragma once

namespace Twin2Engine::Core {
	class Time {
	private:
		static float _updateDeltaTime;

	public:
		static float _timeMultiplier;
		static void Update();
		static float GetTime();
		static float GetDeltaTime();
	};
}