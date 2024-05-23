#pragma once

#include <core/Component.h>
#include <soloud.h>

using namespace SoLoud;
using namespace std;

namespace Twin2Engine::Core {
	class AudioComponent : public Component {
	private:
		string _audioName = "";
		size_t _audioId = 0;
		handle _audioHandle = 0;

		bool _loaded = false;
		bool _loop = false;
		float _volume = 1.0f;

#if _DEBUG
		// For ImGui
		bool _fileDialogOpen;
		ImFileDialogInfo _fileDialogInfo;
#endif
	public:

		void SetAudio(string path);
		void SetAudio(size_t id);
		void Play();
		void Pause();
		void Stop();
		void Loop();
		void UnLoop();
		void SetPlayPosition(SoLoud::time seconds);
		void SetVolume(float value);
		SoLoud::time GetAudioLength();
		SoLoud::time GetPlayPosition();
		SoLoud::time GetPlayTime();
		float GetVolume();
		bool IsPaused();
		bool IsLooping();

		/*
		virtual void Initialize();
		virtual void Update();
		virtual void OnEnable();
		virtual void OnDisable();
		*/
		void OnDestroy() override;
		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;
#if _DEBUG
		virtual void DrawEditor() override;
#endif
	};
}