#pragma once

#include <map>
#include <string>
#include <soloud.h>
#include <soloud_wav.h>

using namespace SoLoud;
using namespace std;

namespace Twin2Engine::Manager {
	class AudioManager {
	private:
		static Soloud _soloud;
		static map<size_t, Wav> _loadedAudio;

	public:
		static size_t LoadAudio(string path);

		static handle PlayAudio(string path);
		static void PauseAudio(handle h);
		static void ResumeAudio(handle h);
		static void StopAudio(handle h); // delete handle

		static void SetPositionAudio(handle h, time seconds);
		static void SetVolume(handle h, float value);
		static void SetLooping(handle h, bool loop);

		static float GetVolume(handle h);
		static bool IsPaused(handle h);
		static bool IsHandleValid(handle h);
		static bool IsLooping(handle h);

		static void FadeVolume(handle h, float to, time seconds);

		//static void UnloadAll();
	};
}