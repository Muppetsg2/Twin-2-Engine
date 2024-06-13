#include <manager/AudioManager.h>
#include <spdlog/spdlog.h>
#include <filesystem>

#if _DEBUG
#include <regex>
#endif

using namespace Twin2Engine::Manager;

Soloud AudioManager::_soloud = Soloud();
map<size_t, Wav*> AudioManager::_loadedAudio = map<size_t, Wav*>();

#if _DEBUG
bool AudioManager::_fileDialogOpen = false;
ImFileDialogInfo AudioManager::_fileDialogInfo;
#endif

map<size_t, string> AudioManager::_audiosPaths;

bool AudioManager::_init = false;

void AudioManager::UnloadAudio(size_t id)
{
    if (_loadedAudio.count(id) == 0) {
        SPDLOG_ERROR("AudioManager::Audio not found");
        return;
    }

    if (_init) {
        StopWav(id);
    }

    delete _loadedAudio[id];
    _loadedAudio.erase(id);
    _audiosPaths.erase(id);
}

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
            SPDLOG_ERROR(_soloud.getErrorString(res));
            return 0;
        }
        else {
            _init = true;
        }
    }

    if (_loadedAudio.find(h) != _loadedAudio.end()) {
        return h;
    }

    if (filesystem::exists(path))
    {
        Wav* sample = new Wav();
        result res = sample->load(path.c_str());
        if (res != 0) {
            SPDLOG_ERROR(_soloud.getErrorString(res));
            return 0;
        }

        _loadedAudio[h] = sample;
        _audiosPaths[h] = path;

        return h;
    }
    else
    {
        SPDLOG_ERROR("AudioManager::Audio file '{0}' not found!", path);
        return 0;
    }
}

handle AudioManager::GetAudioHandle(string path)
{
    if (!_init) {
        SPDLOG_ERROR("AudioManager::SoLoud engine not initialized");
        return 0;
    }

    size_t h = LoadAudio(path);
    return h != 0 ? _soloud.play(*_loadedAudio[h], -1.f, 0.f, true) : 0;
}

handle AudioManager::GetAudioHandle(size_t id)
{
    if (!_init) {
        SPDLOG_ERROR("AudioManager::SoLoud engine not initialized");
        return 0;
    }

    if (_loadedAudio.count(id) == 0) {
        SPDLOG_ERROR("AudioManager::Audio not found");
        return 0;
    }

    return _soloud.play(*_loadedAudio[id], -1.f, 0.f, true);
}

handle AudioManager::PlayAudioInstance(string path)
{
    if (!_init) {
        SPDLOG_ERROR("AudioManager::SoLoud engine not initialized");
        return 0;
    }

    size_t h = LoadAudio(path);
    return h != 0 ? _soloud.play(*_loadedAudio[h]) : 0;
}

handle AudioManager::PlayAudioInstance(size_t id)
{
    if (!_init) {
        SPDLOG_ERROR("AudioManager::SoLoud engine not initialized");
        return 0;
    }

    if (_loadedAudio.count(id) == 0) {
        SPDLOG_ERROR("AudioManager::Audio not found");
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
        SPDLOG_ERROR("AudioManager::SoLoud engine not initialized");
        return;
    }

    if (IsHandleValid(h)) {
        _soloud.setPause(h, true);
    }
    else {
        SPDLOG_ERROR("AudioManager::Handle Not Valid");
    }
}

void AudioManager::ResumeAudio(handle h)
{
    if (!_init) {
        SPDLOG_ERROR("AudioManager::SoLoud engine not initialized");
        return;
    }

    if (IsHandleValid(h)) {
        _soloud.setPause(h, false);
    }
    else {
        SPDLOG_ERROR("AudioManager::Handle Not Valid");
    }
}

void AudioManager::StopAudio(handle h)
{
    if (!_init) {
        SPDLOG_ERROR("AudioManager::SoLoud engine not initialized");
        return;
    }

    if (IsHandleValid(h)) {
        _soloud.stop(h);
    }
    else {
        SPDLOG_ERROR("AudioManager::Handle Not Valid");
    }
}

void AudioManager::StopWav(string path)
{
    StopWav(hash<string>{}(path));
}

void AudioManager::StopWav(size_t id)
{
    if (!_init) {
        SPDLOG_ERROR("AudioManager::SoLoud engine not initialized");
        return;
    }

    if (_loadedAudio.count(id) == 0) {
        SPDLOG_ERROR("AudioManager::Audio not found");
        return;
    }

    _soloud.stopAudioSource(*_loadedAudio[id]);
}

void AudioManager::StopAll()
{
    _soloud.stopAll();
}

void AudioManager::SetPositionAudio(handle h, SoLoud::time seconds)
{
    if (!_init) {
        SPDLOG_ERROR("AudioManager::SoLoud engine not initialized");
        return;
    }

    if (IsHandleValid(h)) {
        _soloud.seek(h, seconds);
    }
    else {
        SPDLOG_ERROR("AudioManager::Handle Not Valid");
    }
}

void AudioManager::SetVolume(handle h, float value)
{
    if (!_init) {
        SPDLOG_ERROR("AudioManager::SoLoud engine not initialized");
        return;
    }

    if (IsHandleValid(h)) {
        _soloud.setVolume(h, value);
    }
    else {
        SPDLOG_ERROR("AudioManager::Handle Not Valid");
    }
}

void AudioManager::SetLooping(handle h, bool loop)
{
    if (!_init) {
        SPDLOG_ERROR("AudioManager::SoLoud engine not initialized");
        return;
    }

    if (IsHandleValid(h)) {
        _soloud.setLooping(h, loop);
    }
    else {
        SPDLOG_ERROR("AudioManager::Handle Not Valid");
    }
}

string AudioManager::GetAudioName(string path)
{
    size_t h = hash<string>{}(path);
    return GetAudioName(h);
}

string AudioManager::GetAudioName(size_t id)
{
    if (_loadedAudio.count(id) == 0) {
        SPDLOG_ERROR("AudioManager::Audio not found");
        return "";
    }

    string p = _audiosPaths[id];
    return std::filesystem::path(p).stem().string();
}

float AudioManager::GetVolume(handle h)
{
    if (!_init) {
        SPDLOG_ERROR("AudioManager::SoLoud engine not initialized");
        return 0.f;
    }

    if (!IsHandleValid(h)) {
        SPDLOG_ERROR("AudioManager::Handle Not Valid");
        return 0.f;
    }

    return _soloud.getVolume(h);
}

SoLoud::time AudioManager::GetAudioTime(string path)
{
    if (!_init) {
        SPDLOG_ERROR("AudioManager::SoLoud engine not initialized");
        return SoLoud::time();
    }

    size_t h = hash<string>{}(path);

    if (_loadedAudio.count(h) == 0) {
        SPDLOG_ERROR("AudioManager::Audio not found");
        return SoLoud::time();
    }

    return _loadedAudio[h]->getLength();
}

SoLoud::time AudioManager::GetAudioTime(size_t id)
{
    if (!_init) {
        SPDLOG_ERROR("AudioManager::SoLoud engine not initialized");
        return SoLoud::time();
    }

    if (_loadedAudio.count(id) == 0) {
        SPDLOG_ERROR("AudioManager::Audio not found");
        return SoLoud::time();
    }

    return _loadedAudio[id]->getLength();
}

bool AudioManager::IsPaused(handle h)
{
    if (!_init) {
        SPDLOG_ERROR("AudioManager::SoLoud engine not initialized");
        return false;
    }

    if (!IsHandleValid(h)) {
        SPDLOG_ERROR("AudioManager::Handle Not Valid");
        return false;
    }

    return _soloud.getPause(h);
}

bool AudioManager::IsHandleValid(handle h)
{
    if (!_init) {
        SPDLOG_ERROR("AudioManager::SoLoud engine not initialized");
        return false;
    }

    return _soloud.isValidVoiceHandle(h);
}

bool AudioManager::IsAudioValid(size_t id)
{
    return _loadedAudio.contains(id);
}

SoLoud::time AudioManager::GetPlayTime(handle h)
{
    if (!_init) {
        SPDLOG_ERROR("AudioManager::SoLoud engine not initialized");
        return SoLoud::time();
    }

    if (!IsHandleValid(h)) {
        SPDLOG_ERROR("AudioManager::Handle Not Valid");
        return SoLoud::time();
    }

    return _soloud.getStreamTime(h);
}

SoLoud::time AudioManager::GetPlayPosition(handle h)
{
    if (!_init) {
        SPDLOG_ERROR("AudioManager::SoLoud engine not initialized");
        return SoLoud::time();
    }

    if (!IsHandleValid(h)) {
        SPDLOG_ERROR("AudioManager::Handle Not Valid");
        return SoLoud::time();
    }

    return _soloud.getStreamPosition(h);
}

bool AudioManager::IsLooping(handle h)
{
    if (!_init) {
        SPDLOG_ERROR("AudioManager::SoLoud engine not initialized");
        return false;
    }

    if (!IsHandleValid(h)) {
        SPDLOG_ERROR("AudioManager::Handle Not Valid");
        return false;
    }

    return _soloud.getLooping(h);
}

void AudioManager::FadeVolume(handle h, float to, SoLoud::time seconds)
{
    if (!_init) {
        SPDLOG_ERROR("AudioManager::SoLoud engine not initialized");
        return;
    }

    if (IsHandleValid(h)) {
        _soloud.fadeVolume(h, to, seconds);
    }
    else {
        SPDLOG_ERROR("AudioManager::Handle Not Valid");
    }
}

void AudioManager::UnloadAll()
{
    _soloud.stopAll();

    for (auto& ad : _loadedAudio) {
        delete ad.second;
    }

    _loadedAudio.clear();
    _audiosPaths.clear();
    _soloud.deinit();
    _init = false;
}

std::map<size_t, string> AudioManager::GetAllAudiosNames()
{
    std::map<size_t, std::string> names = std::map<size_t, std::string>();

    for (auto item : _audiosPaths) {
        names[item.first] = std::filesystem::path(item.second).stem().string();
    }
    return names;
}

YAML::Node AudioManager::Serialize()
{
    YAML::Node audios;
    size_t id = 0;
    for (const auto& audioPair : _audiosPaths) {
        YAML::Node audio;
        audio["id"] = id++;
        audio["path"] = audioPair.second;
        audios.push_back(audio);
    }
    return audios;
}

#if _DEBUG
void AudioManager::DrawEditor(bool* p_open)
{
    if (!ImGui::Begin("Audio Manager", p_open)) {
        ImGui::End();
        return;
    }

    ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    bool node_open = ImGui::TreeNodeEx(string("Audios##Audio Manager").c_str(), node_flag);

    std::list<size_t> clicked = std::list<size_t>();
    clicked.clear();
    if (node_open) {
        int i = 0;
        for (auto& item : _audiosPaths) {
            string n = GetAudioName(item.second);
            ImGui::BulletText(n.c_str());
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 10);
            if (ImGui::Button(string(ICON_FA_TRASH_CAN "##Remove Audio Manager").append(std::to_string(i)).c_str())) {
                clicked.push_back(item.first);
            }
            ++i;
        }
        ImGui::TreePop();
    }

    if (clicked.size() > 0) {
        clicked.sort();

        for (int i = clicked.size() - 1; i > -1; --i)
        {
            UnloadAudio(clicked.back());

            clicked.pop_back();
        }
    }

    clicked.clear();

    if (ImGui::Button("Load Audio##Audio Manager", ImVec2(ImGui::GetContentRegionAvail().x, 0.f))) {
        _fileDialogOpen = true;
        _fileDialogInfo.type = ImGuiFileDialogType_OpenFile;
        _fileDialogInfo.title = "Open File##Audio Manager";
        _fileDialogInfo.directoryPath = std::filesystem::path(std::filesystem::current_path().string() + "\\res\\music");
    }

    if (ImGui::FileDialog(&_fileDialogOpen, &_fileDialogInfo))
    {
        // Result path in: m_fileDialogInfo.resultPath
        string path = std::filesystem::relative(_fileDialogInfo.resultPath).string();

        if (std::regex_search(path, std::regex("(?:[/\\\\]res[/\\\\])"))) {

            LoadAudio(path.substr(path.find("res")));
        }
        else {
            LoadAudio(path);
        }
    }

    ImGui::End();
}
#endif