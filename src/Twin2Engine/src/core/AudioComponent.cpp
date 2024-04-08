#include <core/AudioComponent.h>
#include <manager/AudioManager.h>
#include <spdlog/spdlog.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

// ZROBI� BY POBIERA� HANDLE KIEDY TO POTRZEBNE (POZWALA TO NA ZARZ�DZANIE ZASOBAMI PRZEZ SCENE MANAGER)

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
	}

	return AudioManager::IsLooping(_audioHandle);
}

void AudioComponent::OnDestroy()
{
	if (AudioManager::IsHandleValid(_audioHandle)) {
		AudioManager::StopAudio(_audioHandle);
	}
}
