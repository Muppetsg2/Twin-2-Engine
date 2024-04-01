#pragma once

namespace Twin2Engine::Core {
	class Time {
	private:
		static float _updateDeltaTime;

	public:
		static void Update();
		static float GetDeltaTime();
	};
}