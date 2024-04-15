#include <core/AudioComponent.h>
#include <manager/AudioManager.h>
#include <manager/SceneManager.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

void AudioComponent::SetAudio(string path)
{
	_audioId = hash<string>{}(path);
	_audioHandle = AudioManager::GetAudioHandle(path);
	_loaded = true;
}

void AudioComponent::SetAudio(size_t id)
{
	_audioId = id;
	_audioHandle = AudioManager::GetAudioHandle(id);
	_loaded = true;
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
		spdlog::error("AudioComponent::Audio Was Not Loaded!");
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
		spdlog::error("AudioComponent::Audio Was Not Loaded!");
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
		spdlog::error("AudioComponent::Audio Was Not Loaded!");
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
		spdlog::error("AudioComponent::Audio Was Not Loaded!");
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
		spdlog::error("AudioComponent::Audio Was Not Loaded!");
	}
}

void AudioComponent::SetTimePosition(SoLoud::time seconds)
{
	if (_loaded) {
		if (!AudioManager::IsHandleValid(_audioHandle)) {
			_audioHandle = AudioManager::GetAudioHandle(_audioId);
		}

		AudioManager::SetPositionAudio(_audioHandle, seconds);
		return;
	}
	else {
		spdlog::error("AudioComponent::Audio Was Not Loaded!");
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
		spdlog::error("AudioComponent::Audio Was Not Loaded!");
	}
}

SoLoud::time AudioComponent::GetAudioLength()
{
	if (_loaded) {
		return AudioManager::GetAudioTime(_audioId);
	}
	else {
		spdlog::error("AudioComponent::Audio Was Not Loaded!");
		return SoLoud::time();
	}
}

SoLoud::time AudioComponent::GetPlayPosition()
{
	if (!_loaded) {
		spdlog::error("AudioComponent::Audio Was Not Loaded!");
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
		spdlog::error("AudioComponent::Audio Was Not Loaded!");
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
		spdlog::error("AudioComponent::Audio Was Not Loaded!");
		return 0.f;
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
		spdlog::error("AudioComponent::Audio Was Not Loaded!");
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
		spdlog::error("AudioComponent::Audio Was Not Loaded!");
		return false;
	}

	if (!AudioManager::IsHandleValid(_audioHandle)) {
		_audioHandle = AudioManager::GetAudioHandle(_audioId);
		AudioManager::SetLooping(_audioHandle, _loop);
	}

	return AudioManager::IsLooping(_audioHandle);
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
	node.remove("subTypes");
	node["audio"] = SceneManager::GetAudioSaveIdx(_audioId);
	node["loop"] = _loop;
	node["volume"] = _volume;
	return node;
}