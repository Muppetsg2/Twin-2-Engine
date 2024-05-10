#pragma once

#include <map>
#include <string>
#include <soloud.h>
#include <soloud_wav.h>

using namespace SoLoud;
using namespace std;

namespace Twin2Engine::Manager {
	class SceneManager;
	class PrefabManager;

	class AudioManager {
	private:
		static Soloud _soloud;
		static map<size_t, Wav*> _loadedAudio;

		static map<size_t, string> _audiosPaths;

		static bool _init;

		static void UnloadAudio(size_t id);
	public:
		static result Init();
		static const char* GetErrorString(result errorCode);

		static size_t LoadAudio(string path);

		static handle GetAudioHandle(string path);
		static handle GetAudioHandle(size_t id);

		static handle PlayAudioInstance(string path);
		static handle PlayAudioInstance(size_t id);

		static void PlayAudio(handle h); // doing the same as ResumeAudio
		static void PauseAudio(handle h);
		static void ResumeAudio(handle h);

		static void StopAudio(handle h); // delete handle
		static void StopWav(string path);
		static void StopWav(size_t id);
		static void StopAll();

		static void SetPositionAudio(handle h, SoLoud::time seconds);
		static void SetVolume(handle h, float value);
		static void SetLooping(handle h, bool loop);

		static string GetAudioName(string path);
		static string GetAudioName(size_t id);
		static float GetVolume(handle h);
		static SoLoud::time GetAudioTime(string path);
		static SoLoud::time GetAudioTime(size_t id);
		static SoLoud::time GetPlayTime(handle h);
		static SoLoud::time GetPlayPosition(handle h);
		static bool IsPaused(handle h);
		static bool IsHandleValid(handle h);
		static bool IsLooping(handle h);

		static void FadeVolume(handle h, float to, SoLoud::time seconds);

		static void UnloadAll();

		static YAML::Node Serialize();

		friend class SceneManager;
		friend class PrefabManager;
	};
}