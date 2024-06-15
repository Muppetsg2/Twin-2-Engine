#include <core/AudioComponent.h>
#include <manager/AudioManager.h>
#include <manager/SceneManager.h>
#include <format>
#include <cmath>
#include <core/MathExtensions.h>
#include <tools/templates.h>
#include <tools/YamlConverters.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

void AudioComponent::SetAudio(string path)
{
	_audioId = hash<string>{}(path);
	_audioHandle = AudioManager::GetAudioHandle(path);
	_loaded = true;
	AudioManager::SetLooping(_audioHandle, _loop);
	AudioManager::SetVolume(_audioHandle, _volume);
	_audioName = AudioManager::GetAudioName(_audioId);
}

void AudioComponent::SetAudio(size_t id)
{
	_audioId = id;
	_audioHandle = AudioManager::GetAudioHandle(id);
	_loaded = true;
	AudioManager::SetLooping(_audioHandle, _loop);
	AudioManager::SetVolume(_audioHandle, _volume);
	_audioName = AudioManager::GetAudioName(_audioId);
}

void AudioComponent::Play()
{
	if (_loaded) {
		if (AudioManager::IsHandleValid(_audioHandle)) {
			AudioManager::PlayAudio(_audioHandle);
			return;
		}
		else {
			_audioHandle = AudioManager::PlayAudioInstance(_audioId);
			return;
		}
	}
	else {
		SPDLOG_WARN("AudioComponent::Audio Was Not Loaded!");
	}
}

void AudioComponent::Pause()
{
	if (_loaded) {
		if (AudioManager::IsHandleValid(_audioHandle)) {
			AudioManager::PauseAudio(_audioHandle);
			return;
		}
		else {
			_audioHandle = AudioManager::GetAudioHandle(_audioId);
			return;
		}
	}
	else {
		SPDLOG_WARN("AudioComponent::Audio Was Not Loaded!");
	}
}

void AudioComponent::Stop()
{
	if (_loaded) {
		if (AudioManager::IsHandleValid(_audioHandle)) {
			AudioManager::StopAudio(_audioHandle);
			_audioHandle = AudioManager::GetAudioHandle(_audioId);
			AudioManager::SetLooping(_audioHandle, _loop);
			AudioManager::SetVolume(_audioHandle, _volume);
			return;
		}
	}
	else {
		SPDLOG_WARN("AudioComponent::Audio Was Not Loaded!");
	}
}

void AudioComponent::Loop()
{
	if (_loaded) {
		if (!AudioManager::IsHandleValid(_audioHandle)) {
			_audioHandle = AudioManager::GetAudioHandle(_audioId);
		}

		AudioManager::SetLooping(_audioHandle, true);
		_loop = true;
		return;
	}
	else {
		SPDLOG_WARN("AudioComponent::Audio Was Not Loaded!");
		_loop = true;
	}
}

void AudioComponent::UnLoop()
{
	if (_loaded) {
		if (!AudioManager::IsHandleValid(_audioHandle)) {
			_audioHandle = AudioManager::GetAudioHandle(_audioId);
		}

		AudioManager::SetLooping(_audioHandle, false);
		_loop = false;
		return;
	}
	else {
		SPDLOG_WARN("AudioComponent::Audio Was Not Loaded!");
		_loop = false;
	}
}

void AudioComponent::SetPlayPosition(SoLoud::time seconds)
{
	if (_loaded) {
		if (!AudioManager::IsHandleValid(_audioHandle)) {
			_audioHandle = AudioManager::GetAudioHandle(_audioId);
		}

		AudioManager::SetPositionAudio(_audioHandle, seconds);
		return;
	}
	else {
		SPDLOG_WARN("AudioComponent::Audio Was Not Loaded!");
	}
}

void AudioComponent::SetVolume(float value)
{
	if (_loaded) {
		if (!AudioManager::IsHandleValid(_audioHandle)) {
			_audioHandle = AudioManager::GetAudioHandle(_audioId);
		}

		AudioManager::SetVolume(_audioHandle, value);
		_volume = value;
		return;
	}
	else {
		SPDLOG_WARN("AudioComponent::Audio Was Not Loaded!");
		_volume = value;
	}
}

void AudioComponent::SetPlayOnStart(bool value)
{
	if (_playOnStart != value) {
		_playOnStart = value;
	}
}

size_t AudioComponent::GetAudio()
{
	return _audioId;
}

string AudioComponent::GetAudioName()
{
	return _audioName;
}

SoLoud::time AudioComponent::GetAudioLength()
{
	if (_loaded) {
		return AudioManager::GetAudioTime(_audioId);
	}
	else {
		SPDLOG_WARN("AudioComponent::Audio Was Not Loaded!");
		return SoLoud::time();
	}
}

SoLoud::time AudioComponent::GetPlayPosition()
{
	if (!_loaded) {
		SPDLOG_WARN("AudioComponent::Audio Was Not Loaded!");
		return SoLoud::time();
	}

	if (!AudioManager::IsHandleValid(_audioHandle)) {
		_audioHandle = AudioManager::GetAudioHandle(_audioId);
	}

	return AudioManager::GetPlayPosition(_audioHandle);
}

SoLoud::time AudioComponent::GetPlayTime()
{
	if (!_loaded) {
		SPDLOG_WARN("AudioComponent::Audio Was Not Loaded!");
		return SoLoud::time();
	}

	if (!AudioManager::IsHandleValid(_audioHandle)) {
		_audioHandle = AudioManager::GetAudioHandle(_audioId);
	}

	return AudioManager::GetPlayTime(_audioHandle);
}

float AudioComponent::GetVolume()
{
	if (!_loaded) {
		SPDLOG_WARN("AudioComponent::Audio Was Not Loaded!");
		return _volume;
	}

	if (!AudioManager::IsHandleValid(_audioHandle)) {
		_audioHandle = AudioManager::GetAudioHandle(_audioId);
		AudioManager::SetVolume(_audioHandle, _volume);
	}

	return AudioManager::GetVolume(_audioHandle);
}

bool AudioComponent::IsPaused()
{
	if (!_loaded) {
		SPDLOG_ERROR("AudioComponent::Audio Was Not Loaded!");
		return false;
	}

	if (!AudioManager::IsHandleValid(_audioHandle)) {
		_audioHandle = AudioManager::GetAudioHandle(_audioId);
	}

	return AudioManager::IsPaused(_audioHandle);
}

bool AudioComponent::IsLooping()
{
	if (!_loaded) {
		SPDLOG_ERROR("AudioComponent::Audio Was Not Loaded!");
		return _loop;
	}

	if (!AudioManager::IsHandleValid(_audioHandle)) {
		_audioHandle = AudioManager::GetAudioHandle(_audioId);
		AudioManager::SetLooping(_audioHandle, _loop);
	}

	return AudioManager::IsLooping(_audioHandle);
}

bool AudioComponent::IsPlayOnStartSet()
{
	return _playOnStart;
}

void AudioComponent::Initialize()
{
	if (_playOnStart) {
		Play();
	}
}

void AudioComponent::OnDisable()
{
	if (_loaded) {
		if (!IsPaused()) Pause();
	}
}

void AudioComponent::OnDestroy()
{
	if (AudioManager::IsHandleValid(_audioHandle)) {
		AudioManager::StopAudio(_audioHandle);
	}
}

YAML::Node AudioComponent::Serialize() const
{
	YAML::Node node = Component::Serialize();
	node["type"] = "Audio";
	node["audio"] = SceneManager::GetAudioSaveIdx(_audioId);
	node["loop"] = _loop;
	node["volume"] = _volume;
	node["playOnStart"] = _playOnStart;
	return node;
}

bool AudioComponent::Deserialize(const YAML::Node& node)
{
	if (!node["audio"] || !node["loop"] || !node["volume"] || !node["playOnStart"] ||
		!Component::Deserialize(node)) return false;

	_loop = node["loop"].as<bool>();
	_volume = node["volume"].as<float>();
	_playOnStart = node["playOnStart"].as<bool>();

	SetAudio(SceneManager::GetAudio(node["audio"].as<size_t>()));

	return true;
}

#if _DEBUG
void AudioComponent::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Audio##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {

		if (Component::DrawInheritedFields()) return;
		std::map<size_t, string> audioNames = AudioManager::GetAllAudiosNames();

		audioNames.insert(std::pair(0, "None"));

		if (!audioNames.contains(_audioId)) {
			_audioId = 0;
			_loaded = false;
			_audioHandle = handle();
		}

		if (ImGui::BeginCombo(string("Audio##").append(id).c_str(), audioNames[_audioId].c_str())) {

			bool clicked = false;
			size_t choosed = _audioId;
			for (auto& item : audioNames) {

				if (ImGui::Selectable(std::string(item.second).append("##").append(id).append(std::to_string(item.first)).c_str(), item.first == _audioId)) {

					if (clicked) continue;

					choosed = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed != 0) {
					SetAudio(choosed);
				}
				else {
					_audioId = 0;
					_loaded = false;
					_audioHandle = handle();
				}
			}

			ImGui::EndCombo();
		}

		if (this->_loaded) {

			if (ImGui::ArrowButton(string("Play Song##").append(id).c_str(), ImGuiDir_Right)) {
				this->Play();
			}
			ImGui::SameLine();
			if (ImGui::PauseButton(string("Pause Song##").append(id).c_str(), 1.f)) {
				this->Pause();
			}
			ImGui::SameLine();
			if (ImGui::StopButton(string("Stop Song##").append(id).c_str(), 1.f)) {
				this->Stop();
			}

			float pos = (float)this->GetPlayPosition();

			float len = (float)this->GetAudioLength();

			ImGui::Text("Play Time: %02.0f:%02.0f", std::floor(this->GetPlayTime() / 60.0), mod(this->GetPlayTime(), 60));
			ImGui::Text("Position: %02.0f:%02.0f / %02.0f:%02.0f", std::floor(pos / 60.f), mod((double)pos, 60.0), std::floor(len / 60.f), mod((double)len, 60.0));

			if (ImGui::SliderFloat(string("Position Slider##").append(id).c_str(), &pos, 0.f, len, std::vformat(string_view("{:02.0f}:{:02.0f}"),
				make_format_args(
					Twin2Engine::Tools::unmove(std::floor((double)pos / 60.0)),
					Twin2Engine::Tools::unmove(mod((double)pos, 60.0))
				)).c_str()))
			{
				this->SetPlayPosition((double)pos);
			}
		}

		float vol = this->_volume;

		ImGui::SliderFloat(string("Volume##").append(id).c_str(), &vol, 0.f, 1.f);

		if (this->_volume != vol) {
			this->SetVolume(vol);
		}

		bool v = this->_loop;

		if (ImGui::Checkbox(string("Loop##").append(id).c_str(), &v)) {
			if (v) {
				if (!this->_loop) {
					this->Loop();
				}
			}
			else {
				if (this->_loop) {
					this->UnLoop();
				}
			}
		}

		v = this->_playOnStart;

		ImGui::Checkbox(string("Play On Start##").append(id).c_str(), &v);

		if (v != this->_playOnStart) {
			this->_playOnStart = v;
		}
	}
}
#endif