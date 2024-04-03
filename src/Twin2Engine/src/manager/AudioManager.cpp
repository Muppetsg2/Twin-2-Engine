#include <manager/AudioManager.h>
#include <spdlog/spdlog.h>

using namespace Twin2Engine::Manager;

Soloud AudioManager::_soloud = Soloud();
map<size_t, Wav*> AudioManager::_loadedAudio = map<size_t, Wav*>();

bool AudioManager::_init = false;

result AudioManager::Init()
{
    if (!_init) {
        SoLoud::result res = _soloud.init();
        if (res == 0) {
            _init = true;
        }
        return res;
    }
    return 0;
}

const char* AudioManager::GetErrorString(result errorCode)
{
    return _soloud.getErrorString(errorCode);
}

size_t AudioManager::LoadAudio(string path)
{
    size_t h = hash<string>{}(path);

    if (_loadedAudio.size() == 0 && !_init) {
        SoLoud::result res = _soloud.init();
        if (res != 0) {
            spdlog::error(_soloud.getErrorString(res));
            return 0;
        }
        else {
            _init = true;
        }
    }

    if (_loadedAudio.find(h) != _loadedAudio.end()) {
        return h;
    }

    Wav* sample = new Wav();
    result res = sample->load(path.c_str());
    if (res != 0) {
        spdlog::error(_soloud.getErrorString(res));
        return 0;
    }

    _loadedAudio[h] = sample;

    return h;
}

handle AudioManager::GetAudioHandle(string path)
{
    if (!_init) {
        spdlog::error("AudioManager::SoLoud engine not initialized");
        return 0;
    }

    size_t h = LoadAudio(path);
    return h != 0 ? _soloud.play(*_loadedAudio[h], -1.f, 0.f, true) : 0;
}

handle AudioManager::GetAudioHandle(size_t id)
{
    if (!_init) {
        spdlog::error("AudioManager::SoLoud engine not initialized");
        return 0;
    }

    if (_loadedAudio.count(id) == 0) {
        spdlog::error("AudioManager::Audio not found");
        return SoLoud::time();
    }

    return _soloud.play(*_loadedAudio[id], -1.f, 0.f, true);
}

handle AudioManager::PlayAudioInstance(string path)
{
    if (!_init) {
        spdlog::error("AudioManager::SoLoud engine not initialized");
        return 0;
    }

    size_t h = LoadAudio(path);
    return h != 0 ? _soloud.play(*_loadedAudio[h]) : 0;
}

handle AudioManager::PlayAudioInstance(size_t id)
{
    if (!_init) {
        spdlog::error("AudioManager::SoLoud engine not initialized");
        return 0;
    }

    if (_loadedAudio.count(id) == 0) {
        spdlog::error("AudioManager::Audio not found");
        return 0;
    }

    return _soloud.play(*_loadedAudio[id]);
}

void AudioManager::PlayAudio(handle h)
{
    ResumeAudio(h);
}

void AudioManager::PauseAudio(handle h)
{
    if (!_init) {
        spdlog::error("AudioManager::SoLoud engine not initialized");
        return;
    }

    if (IsHandleValid(h)) {
        _soloud.setPause(h, true);
    }
    else {
        spdlog::error("AudioManager::Handle Not Valid");
    }
}

void AudioManager::ResumeAudio(handle h)
{
    if (!_init) {
        spdlog::error("AudioManager::SoLoud engine not initialized");
        return;
    }

    if (IsHandleValid(h)) {
        _soloud.setPause(h, false);
    }
    else {
        spdlog::error("AudioManager::Handle Not Valid");
    }
}

void AudioManager::StopAudio(handle h)
{
    if (!_init) {
        spdlog::error("AudioManager::SoLoud engine not initialized");
        return;
    }

    if (IsHandleValid(h)) {
        _soloud.stop(h);
    }
    else {
        spdlog::error("AudioManager::Handle Not Valid");
    }
}

void AudioManager::SetPositionAudio(handle h, SoLoud::time seconds)
{
    if (!_init) {
        spdlog::error("AudioManager::SoLoud engine not initialized");
        return;
    }

    if (IsHandleValid(h)) {
        _soloud.seek(h, seconds);
    }
    else {
        spdlog::error("AudioManager::Handle Not Valid");
    }
}

void AudioManager::SetVolume(handle h, float value)
{
    if (!_init) {
        spdlog::error("AudioManager::SoLoud engine not initialized");
        return;
    }

    if (IsHandleValid(h)) {
        _soloud.setVolume(h, value);
    }
    else {
        spdlog::error("AudioManager::Handle Not Valid");
    }
}

void AudioManager::SetLooping(handle h, bool loop)
{
    if (!_init) {
        spdlog::error("AudioManager::SoLoud engine not initialized");
        return;
    }

    if (IsHandleValid(h)) {
        _soloud.setLooping(h, loop);
    }
    else {
        spdlog::error("AudioManager::Handle Not Valid");
    }
}

float AudioManager::GetVolume(handle h)
{
    if (!_init) {
        spdlog::error("AudioManager::SoLoud engine not initialized");
        return 0.f;
    }

    if (!IsHandleValid(h)) {
        spdlog::error("AudioManager::Handle Not Valid");
        return 0.f;
    }

    return _soloud.getVolume(h);
}

SoLoud::time AudioManager::GetAudioTime(string path)
{
    if (!_init) {
        spdlog::error("AudioManager::SoLoud engine not initialized");
        return SoLoud::time();
    }

    size_t h = hash<string>{}(path);

    if (_loadedAudio.count(h) == 0) {
        spdlog::error("AudioManager::Audio not found");
        return SoLoud::time();
    }

    return _loadedAudio[h]->getLength();
}

SoLoud::time AudioManager::GetAudioTime(size_t id)
{
    if (!_init) {
        spdlog::error("AudioManager::SoLoud engine not initialized");
        return SoLoud::time();
    }

    if (_loadedAudio.count(id) == 0) {
        spdlog::error("AudioManager::Audio not found");
        return SoLoud::time();
    }

    return _loadedAudio[id]->getLength();
}

bool AudioManager::IsPaused(handle h)
{
    if (!_init) {
        spdlog::error("AudioManager::SoLoud engine not initialized");
        return false;
    }

    if (!IsHandleValid(h)) {
        spdlog::error("AudioManager::Handle Not Valid");
        return false;
    }

    return _soloud.getPause(h);
}

bool AudioManager::IsHandleValid(handle h)
{
    if (!_init) {
        spdlog::error("AudioManager::SoLoud engine not initialized");
        return false;
    }

    return _soloud.isValidVoiceHandle(h);
}

SoLoud::time AudioManager::GetPlayTime(handle h)
{
    if (!_init) {
        spdlog::error("AudioManager::SoLoud engine not initialized");
        return SoLoud::time();
    }

    if (!IsHandleValid(h)) {
        spdlog::error("AudioManager::Handle Not Valid");
        return SoLoud::time();
    }

    return _soloud.getStreamTime(h);
}

SoLoud::time AudioManager::GetPlayPosition(handle h)
{
    if (!_init) {
        spdlog::error("AudioManager::SoLoud engine not initialized");
        return SoLoud::time();
    }

    if (!IsHandleValid(h)) {
        spdlog::error("AudioManager::Handle Not Valid");
        return SoLoud::time();
    }

    return _soloud.getStreamPosition(h);
}

bool AudioManager::IsLooping(handle h)
{
    if (!_init) {
        spdlog::error("AudioManager::SoLoud engine not initialized");
        return false;
    }

    if (!IsHandleValid(h)) {
        spdlog::error("AudioManager::Handle Not Valid");
        return false;
    }

    return _soloud.getLooping(h);
}

void AudioManager::FadeVolume(handle h, float to, SoLoud::time seconds)
{
    if (!_init) {
        spdlog::error("AudioManager::SoLoud engine not initialized");
        return;
    }

    if (IsHandleValid(h)) {
        _soloud.fadeVolume(h, to, seconds);
    }
    else {
        spdlog::error("AudioManager::Handle Not Valid");
    }
}

void AudioManager::UnloadAll()
{
    _soloud.stopAll();

    for (auto& ad : _loadedAudio) {
        delete ad.second;
        ad.second = nullptr;
    }

    _loadedAudio.clear();
    _soloud.deinit();
    _init = false;
}