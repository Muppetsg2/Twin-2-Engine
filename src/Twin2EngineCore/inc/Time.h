#pragma once

namespace Twin2EngineCore {
	class Time {
	private:
		static float _updateDeltaTime;

	public:
		static void Update();
		static float GetDeltaTime();
	};
}