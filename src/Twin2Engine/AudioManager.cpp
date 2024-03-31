#include <manager/AudioManager.h>
#include <spdlog/spdlog.h>

using namespace Twin2Engine::Manager;

size_t AudioManager::LoadAudio(string path)
{
    size_t h = hash<string>{}(path);

    if (_loadedAudio.size() == 0) {
        _soloud.init();
    }

    if (_loadedAudio.find(h) != _loadedAudio.end()) {
        return h;
    }

    Wav sample;
    result res = sample.load(path.c_str());
    if (res != 0) {
        spdlog::error(_soloud.getErrorString(res));
        return 0;
    }

    _loadedAudio[h] = sample;

    return h;
}

handle AudioManager::GetAudioHandle(string path)
{
    size_t h = hash<string>{}(path);
    return _soloud.play(_loadedAudio[h], -1.f, 0.f, true);
}

handle AudioManager::GetAudioHandle(size_t id)
{
    return _soloud.play(_loadedAudio[id], -1.f, 0.f, true);
}

handle AudioManager::PlayAudioInstance(string path)
{
    size_t h = hash<string>{}(path);
    return _soloud.play(_loadedAudio[h]);
}

handle AudioManager::PlayAudioInstance(size_t id)
{
    return _soloud.play(_loadedAudio[id]);
}

void AudioManager::PlayAudio(handle h)
{
    ResumeAudio(h);
}

void AudioManager::PauseAudio(handle h)
{
    if (IsHandleValid(h)) {
        _soloud.setPause(h, true);
    }
}

void AudioManager::ResumeAudio(handle h)
{
    if (IsHandleValid(h)) {
        _soloud.setPause(h, false);
    }
}

void AudioManager::StopAudio(handle h)
{
    if (IsHandleValid(h)) {
        _soloud.stop(h);
    }
}

void AudioManager::SetPositionAudio(handle h, SoLoud::time seconds)
{
    if (IsHandleValid(h)) {
        _soloud.seek(h, seconds);
    }
}

void AudioManager::SetVolume(handle h, float value)
{
    if (IsHandleValid(h)) {
        _soloud.setVolume(h, value);
    }
}

void AudioManager::SetLooping(handle h, bool loop)
{
    if (IsHandleValid(h)) {
        _soloud.setLooping(h, loop);
    }
}

float AudioManager::GetVolume(handle h)
{
    return IsHandleValid(h) ? _soloud.getVolume(h) : 0.f;
}

SoLoud::time AudioManager::GetAudioTime(string path)
{
    size_t h = hash<string>{}(path);
    return _loadedAudio[h].getLength();
}

SoLoud::time AudioManager::GetAudioTime(size_t id)
{
    return _loadedAudio[id].getLength();
}

bool AudioManager::IsPaused(handle h)
{
    return IsHandleValid(h) ? _soloud.getPause(h) : false;
}

bool AudioManager::IsHandleValid(handle h)
{
    return _soloud.isValidVoiceHandle(h);
}

bool AudioManager::IsLooping(handle h)
{
    return IsHandleValid(h) ? _soloud.getLooping(h) : false;
}

void AudioManager::FadeVolume(handle h, float to, SoLoud::time seconds)
{
    if (IsHandleValid(h)) {
        _soloud.fadeVolume(h, to, seconds);
    }
}

void AudioManager::UnloadAll()
{
    _soloud.deinit();
}