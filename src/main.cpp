#define TWIN2_VERSION "1.2"

#define USE_IMGUI_CONSOLE_OUTPUT true
#define USE_WINDOWS_CONSOLE_OUTPUT false

#if USE_IMGUI_CONSOLE_OUTPUT || !USE_WINDOWS_CONSOLE_OUTPUT || !_DEBUG

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#endif

const char* const tracy_ImGuiDrawingConsole = "ImGuiDrawingConsole";
const char* const tracy_RenderingImGui = "RenderingImGui";

#define EDITOR_LOGGER
//#define RELEASE_LOGGER

#include <GameEngine.h>

// GAME SCRIPTS
#include <PlaneGenerator.h>
#include <UIScripts/MinigameManager.h>


// TILEMAP
#include <Tilemap/HexagonalTilemap.h>
#include <Tilemap/HexagonalTile.h>

// HUMANS
#include <Humans/Human.h>
#include <Humans/HumanMovement.h>

using namespace Humans;

// GENERATION
#include <Generation/MapGenerator.h>
#include <Generation/ContentGenerator.h>
#include <Generation/MapRegion.h>
#include <Generation/MapSector.h>
#include <Generation/MapHexTile.h>
#include <Generation/Generators/CitiesGenerator.h>
#include <Generation/Generators/LakeGenerator.h>
#include <Generation/Generators/MountainsGenerator.h>
#include <Generation/Generators/RadioStationGeneratorRegionBased.h>
#include <Generation/Generators/RadioStationGeneratorSectorBased.h>
#include <Generation/Generators/RegionsBySectorsGenerator.h>
#include <Generation/Generators/SectorsGenerator.h>
#include <Generation/Generators/RegionsGeneratorByKMeans.h>
#include <Generation/Generators/SectorGeneratorForRegionsByKMeans.h>

#include <Humans/HumansGenerator.h>

// ASTAR PATHFINDING
#include <AstarPathfinding/AStarPath.h>
#include <AstarPathfinding/AStarPathfinder.h>

using namespace AStar;

// CLOUD CONTROLLER
#include <Clouds/CloudController.h>

// YAML CONVERTERS
#include <tools/YamlConverters.h>
#include <Generation/YamlConverters.h>

// LOGGER
#include <tools/FileLoggerSink.h>

#if _DEBUG
// EDITOR
#include <Editor/Common/MaterialCreator.h>
#include <Editor/Common/ProcessingMtlFiles.h>
//#include <Editor/Common/ScriptableObjectEditorManager.h>
#include <Editor/Common/ImGuiSink.h>
using Editor::Common::ImGuiSink;
using Editor::Common::ImGuiLogMessage;
#endif

using namespace Twin2Engine;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::UI;
using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Physic;

using Tilemap::HexagonalTile;
using Tilemap::HexagonalTilemap;
using namespace Generation;
using namespace Generation::Generators;

//GAMESCRIPTS
#include <MovementController.h>
#include <GameManager.h>
#include <Playable.h>
#include <PlayerMovement.h>
#include <Player.h>
#include <Enemy.h>
#include <EnemyMovement.h>
#include <AreaTaking/GetMoneyFromTiles.h>
#include <GameTimer.h>
#include <ConcertRoad.h>
#include <Abilities/ConcertAbilityController.h>

#include <RadioStation/RadioStation.h>
#include <RadioStation/RadioStationPlayingController.h>

#include <UIScripts/PatronChoicePanelController.h>

using namespace GameScripts;

#include <processes/Coroutine.h>

#pragma region CAMERA_CONTROLLING

glm::vec3 cameraPos(0.f, 2.f, 5.f);

double lastX = 0.0f;
double lastY = 0.0f;

float cameraSpeed = 40.0f;
float sensitivity = 0.1f;

bool mouseNotUsed = true;

#pragma endregion

#pragma region FunctionsDeclaration

void input();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

#if _DEBUG
void init_imgui();
void begin_imgui();
void render_imgui();
void end_imgui();
#endif

#pragma endregion

constexpr const char* WINDOW_NAME = "Twin^2 Engine";
constexpr int32_t WINDOW_WIDTH  = 1920;
constexpr int32_t WINDOW_HEIGHT = 1080;
constexpr bool WINDOW_FULLSCREEN = false;

// Change these to lower GL version like 4.5 if GL 4.6 can't be initialized on your machine
constexpr int32_t GL_VERSION_MAJOR = 4;
constexpr int32_t GL_VERSION_MINOR = 5;
constexpr const char* glsl_version = "#version 450";

#if _DEBUG
// For ImGui
static bool _openLoadSceneWindow = false;
static bool _fileDialogSceneOpen;
static bool _fileDialogSceneSave;
static ImFileDialogInfo _fileDialogSceneOpenInfo;
static ImFileDialogInfo _fileDialogSceneSaveInfo;
#endif

Window* window;

GameObject* Camera;

int main(int, char**)
{
#pragma region Initialization
    
    // LOGGING: SPDLOG INITIALIZATION
#if _DEBUG

#if USE_IMGUI_CONSOLE_OUTPUT
    auto console_sink = std::make_shared<Editor::Common::ImGuiSink<mutex>>("res/logs/log.txt", 100.0f);
#elif USE_WINDOWS_CONSOLE_OUTPUT
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
#else
    spdlog::set_level(spdlog::level::off);
#endif

#if USE_IMGUI_CONSOLE_OUTPUT || USE_WINDOWS_CONSOLE_OUTPUT
    auto logger = std::make_shared<spdlog::logger>("logger", console_sink);
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);
    console_sink->StartLogging();
#endif

#else
    auto fileLoggerSink = std::make_shared<Twin2Engine::Tools::FileLoggerSink<mutex>>("logs.txt", 100.0f);
    auto fileLogger = std::make_shared<spdlog::logger>("FileLogger", fileLoggerSink);
    spdlog::register_logger(fileLogger);
    spdlog::set_default_logger(fileLogger);

    spdlog::set_level(spdlog::level::debug);
    fileLoggerSink->StartLogging();
#endif

    SPDLOG_INFO("Twin^2 Engine Version: {}", TWIN2_VERSION);

#if _DEBUG
    SPDLOG_INFO("Configuration: DEBUG");
#else
    SPDLOG_INFO("Configuration: RELEASE");
#endif

    if (!GameEngine::Init(WINDOW_NAME, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FULLSCREEN, GL_VERSION_MAJOR, GL_VERSION_MINOR))
    {
        spdlog::error("Failed to initialize GameEngine!");
        return EXIT_FAILURE;
    }
    spdlog::info("Initialized GameEngine.");

#if _DEBUG
    init_imgui();
    spdlog::info("Initialized ImGui.");
#endif

    SoLoud::result res = AudioManager::Init();
    if (res != 0) {
        spdlog::error(AudioManager::GetErrorString(res));
        return EXIT_FAILURE;
    }
    spdlog::info("Initialized SoLoud.");

#pragma endregion

    window = Window::GetInstance();

#pragma region TILEMAP_DESERIALIZER

    ADD_COMPONENT("HexagonalTilemap", HexagonalTilemap);

#pragma endregion

#pragma region GENERATION_DESERIALIZER

    ADD_COMPONENT("MapGenerator", MapGenerator);

    ADD_COMPONENT("ContentGenerator", ContentGenerator);

    ADD_COMPONENT("MapHexTile", MapHexTile);

    ADD_COMPONENT("MapRegion", MapRegion);

    ADD_COMPONENT("MapSector", MapSector);

    ADD_COMPONENT("Human", Human);

    ADD_COMPONENT("HumanMovement", HumanMovement);

    ADD_COMPONENT("AStarPathfindingNode", AStarPathfindingNode);

    ADD_COMPONENT("AStarPathfinder", AStarPathfinder);


#pragma endregion


    ADD_COMPONENT("MovementController", MovementController);

    ADD_COMPONENT("GameManager", GameManager);
    ADD_COMPONENT("PlayerMovement", PlayerMovement);
    ADD_COMPONENT("Player", Player);

    ADD_COMPONENT("HexTile", HexTile);

    ADD_COMPONENT("Enemy", Enemy);
    ADD_COMPONENT("EnemyMovement", EnemyMovement);

    ADD_COMPONENT("ConcertAbilityController", ConcertAbilityController);
    ADD_COMPONENT("MoneyGainFromTiles", MoneyGainFromTiles);
    ADD_COMPONENT("GameTimer", GameTimer);


    ADD_COMPONENT("RadioStation", RadioStation);
    ADD_COMPONENT("RadioStationPlayingController", RadioStationPlayingController);


    ADD_COMPONENT("PatronChoicePanelController", PatronChoicePanelController);

#pragma region GAME_SCRIPTS_COMPONENTS

    ADD_COMPONENT("PlaneGenerator", PlaneGenerator);
    ADD_COMPONENT("MinigameManager", MinigameManager);
    ADD_COMPONENT("ConcertRoad", ConcertRoad);

#pragma endregion

    SceneManager::GetOnSceneLoaded() += [](std::string sceneName) -> void {
        CameraComponent* cam = SceneManager::GetRootObject()->GetComponentInChildren<CameraComponent>();

        if (cam == nullptr) {
            std::tuple<GameObject*, CameraComponent*> i = SceneManager::CreateGameObject<CameraComponent>();
            cam = std::get<1>(i);
            std::get<0>(i)->SetName("Camera");
        }

        Camera = cam->GetGameObject();
    };

    // ADDING SCENES
    //SceneManager::AddScene("testScene", "res/scenes/BlankScene.scene");
    //SceneManager::AddScene("testScene", "res/scenes/PatronChoice.scene");
    SceneManager::AddScene("testScene", "res/scenes/procedurallyGenerated.scene");
    //SceneManager::AddScene("testScene", "res/scenes/Making Game UI.scene");
    //SceneManager::AddScene("testScene", "res/scenes/MenuScene.scene");
    //SceneManager::AddScene("testScene", "res/scenes/quickSavedScene.scene");
    //SceneManager::AddScene("testScene", "res/scenes/quickSavedScene_Copy.scene");
    //SceneManager::AddScene("testScene", "res/scenes/ToonShading.scene");
    //SceneManager::AddScene("testScene", "res/scenes/HexTileEditScene.scene");

    SceneManager::LoadScene("testScene");
    SceneManager::Update();

    //SceneManager::SaveScene("res/scenes/ToonShading.scene");
    
    Camera = SceneManager::GetRootObject()->GetComponentInChildren<CameraComponent>()->GetGameObject();


#if _DEBUG
    GameEngine::LateRender += []() -> void {

#if TRACY_PROFILER
        ZoneScoped;
#endif

#if TRACY_PROFILER
        // Draw ImGui
        FrameMarkStart(tracy_RenderingImGui);
#endif

        begin_imgui();
        render_imgui(); // edit this function to add your own ImGui controls
        end_imgui(); // this call effectively renders ImGui

#if TRACY_PROFILER
        FrameMarkEnd(tracy_RenderingImGui);
#endif
    };

    GameEngine::EarlyEnd += []() -> void {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        //Editor::Common::ScriptableObjectEditorManager::End();
    };
#endif
    //GameObject* go = SceneManager::CreateGameObject();
    //GameObject* gochild = SceneManager::CreateGameObject(go->GetTransform());
    //
    //GameObject* gochild2 = SceneManager::CreateGameObject(go->GetTransform());
    //gochild->GetTransform()->AddChild(gochild2->GetTransform());
    //PrefabManager::SaveAsPrefab(go, "ParenChild");



    GameEngine::OnInput += [&]() -> void {
        input();
    };

    GameEngine::Start();

    CloudController::DeleteInstance();

#if _DEBUG

#if USE_IMGUI_CONSOLE_OUTPUT
    console_sink->StopLogging();
#endif

#else
    fileLoggerSink->StopLogging();
#endif
    return 0;
}

void input()
{
    if (Input::IsKeyPressed(KEY::ESCAPE))
    {
        window->Close();
        return;
    }

    CameraComponent* c = CameraComponent::GetMainCamera();


#if _DEBUG
    if (Input::IsKeyDown(KEY::W) && Input::GetCursorState() == CURSOR_STATE::DISABLED)
    {
        Camera->GetTransform()->SetGlobalPosition(Camera->GetTransform()->GetGlobalPosition() + c->GetFrontDir() * cameraSpeed * Time::GetDeltaTime());
    }
    if (Input::IsKeyDown(KEY::S) && Input::GetCursorState() == CURSOR_STATE::DISABLED)
    {
        Camera->GetTransform()->SetGlobalPosition(Camera->GetTransform()->GetGlobalPosition() - c->GetFrontDir() * cameraSpeed * Time::GetDeltaTime());
    }
    if (Input::IsKeyDown(KEY::A) && Input::GetCursorState() == CURSOR_STATE::DISABLED)
    {
        Camera->GetTransform()->SetGlobalPosition(Camera->GetTransform()->GetGlobalPosition() - c->GetRight() * cameraSpeed * Time::GetDeltaTime());
    }
    if (Input::IsKeyDown(KEY::D) && Input::GetCursorState() == CURSOR_STATE::DISABLED)
    {
        Camera->GetTransform()->SetGlobalPosition(Camera->GetTransform()->GetGlobalPosition() + c->GetRight() * cameraSpeed * Time::GetDeltaTime());
    }
#endif

    if (Input::IsKeyPressed(KEY::LEFT_ALT))
    {
        mouseNotUsed = true;
        if (Input::GetCursorState() == CURSOR_STATE::DISABLED)
        {
            Input::ShowCursor();
#if _DEBUG
            glfwSetCursorPosCallback(window->GetWindow(), ImGui_ImplGlfw_CursorPosCallback);
#else
            glfwSetCursorPosCallback(window->GetWindow(), NULL);
#endif
        }
        else
        {
            Input::HideAndLockCursor();
            glfwSetCursorPosCallback(window->GetWindow(), mouse_callback);
        }
    }

#if _DEBUG
    if (Input::IsKeyDown(KEY::LEFT_CONTROL) && Input::IsKeyPressed(KEY::N)) {
        // New Scene
        SceneManager::UnloadCurrent();
        SceneManager::AddScene("New Scene", new Scene());
        SceneManager::LoadScene("New Scene");
    }

    if (Input::IsKeyDown(KEY::LEFT_CONTROL) && Input::IsKeyPressed(KEY::L)) {
        // Load Scene
        //ImGui::OpenPopup("Load Scene##File_Scene_Load_Internal", ImGuiPopupFlags_NoReopen);
        _openLoadSceneWindow = true;
    }

    if (Input::IsKeyDown(KEY::LEFT_CONTROL) && Input::IsKeyDown(KEY::LEFT_SHIFT) && Input::IsKeyPressed(KEY::L)) {
        // Load Scene From File
        _fileDialogSceneOpen = true;
        _fileDialogSceneOpenInfo.type = ImGuiFileDialogType_OpenFile;
        _fileDialogSceneOpenInfo.title = "Load Scene##File_Scene_Load";
        _fileDialogSceneOpenInfo.directoryPath = std::filesystem::path(std::filesystem::current_path().string() + "\\res\\scenes");
    }

    if (Input::IsKeyDown(KEY::LEFT_CONTROL) && Input::IsKeyPressed(KEY::R)) {
        // Reload Scene
        std::string name = SceneManager::GetCurrentSceneName();
        SceneManager::UnloadCurrent();
        SceneManager::LoadScene(name);
        SceneManager::Update();
    }

    if (Input::IsKeyDown(KEY::LEFT_CONTROL) && Input::IsKeyPressed(KEY::S)) {
        // Save Scene
        if (SceneManager::GetCurrentScenePath() != "") SceneManager::SaveScene(SceneManager::GetCurrentScenePath());
        else {
            _fileDialogSceneSave = true;
            _fileDialogSceneSaveInfo.type = ImGuiFileDialogType_SaveFile;
            _fileDialogSceneSaveInfo.title = "Save Scene##File_Scene_Save";
            _fileDialogSceneSaveInfo.directoryPath = std::filesystem::path(std::filesystem::current_path().string() + "\\res\\scenes");
        }
    }

    if (Input::IsKeyDown(KEY::LEFT_CONTROL) && Input::IsKeyDown(KEY::LEFT_SHIFT) && Input::IsKeyPressed(KEY::S)) {
        // Save Scene As...
        _fileDialogSceneSave = true;
        _fileDialogSceneSaveInfo.type = ImGuiFileDialogType_SaveFile;
        _fileDialogSceneSaveInfo.title = "Save Scene##File_Scene_Save";
        _fileDialogSceneSaveInfo.directoryPath = std::filesystem::path(std::filesystem::current_path().string() + "\\res\\scenes");
    }
#endif
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
#if _DEBUG

    if (mouseNotUsed)
    {
        lastX = xpos;
        lastY = ypos;
        mouseNotUsed = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos; // Odwrocone, poniewaz wsporzedne zmieniaja sie od dolu do gory  
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    glm::vec3 rot = Camera->GetTransform()->GetGlobalRotation();

    // YAW = ROT Y
    // PITCH = ROT X
    // ROLL = ROT Z

    rot.x += yoffset;

    if (rot.x > 89.f) {
        rot.x = 89.f;
    }

    if (rot.x < -89.f)
    {
        rot.x = -89.f;
    }

    Camera->GetTransform()->SetGlobalRotation(glm::vec3(rot.x, rot.y + xoffset, rot.z));
#endif 
}

#if _DEBUG
void init_imgui()
{
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls

    io.ConfigDockingTransparentPayload = true;  // Enable Docking Transparent

    ImGui_ImplGlfw_InitForOpenGL(Window::GetInstance()->GetWindow(), true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup style
    ImGui::StyleColorsDark();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    ImFont* font = io.Fonts->AddFontFromFileTTF("./res/fonts/Editor/NotoSans-Regular.ttf", 18.f, nullptr, ImGui::GetGlyphRangesPolish());
    IM_ASSERT(font != NULL);

    // ICONS
    ImFontConfig config;
    config.MergeMode = true;
    config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced
    static const ImWchar icon_ranges_brand[] = { ICON_MIN_FAB, ICON_MAX_FAB, 0 };
    static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    font = io.Fonts->AddFontFromFileTTF("./res/fonts/Editor/" FONT_ICON_FILE_NAME_FAB, 13.0f, &config, icon_ranges_brand);
    IM_ASSERT(font != NULL);
    font = io.Fonts->AddFontFromFileTTF("./res/fonts/Editor/" FONT_ICON_FILE_NAME_FAR, 13.0f, &config, icon_ranges);
    IM_ASSERT(font != NULL);
    font = io.Fonts->AddFontFromFileTTF("./res/fonts/Editor/" FONT_ICON_FILE_NAME_FAS, 13.0f, &config, icon_ranges);
    IM_ASSERT(font != NULL);

    font = io.Fonts->AddFontFromFileTTF("./res/fonts/Editor/NotoSans-Bold.ttf", 18.f, nullptr, ImGui::GetGlyphRangesPolish());
    IM_ASSERT(font != NULL);

    io.Fonts->Build();
}

void begin_imgui()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void render_imgui()
{
#if TRACY_PROFILER
    ZoneScoped;
#endif

    if (Input::GetCursorState() == CURSOR_STATE::NORMAL)
    {
        SceneManager::DrawCurrentSceneEditor();

        if (!ImGui::Begin("Twin^2 Engine", NULL, ImGuiWindowFlags_MenuBar)) {
            ImGui::End();
            return;
        }

        static bool _consoleOpened = true;

        static bool _fontOpened = false;
        static bool _audioOpened = false;
        static bool _materialsOpened = false;
        static bool _texturesOpened = false;
        static bool _spriteOpened = false;
        static bool _modelsOpened = false;
        static bool _prefabOpened = false;
        static bool _scriptableOpened = false;

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File##Menu"))
            {
                if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
                    SceneManager::UnloadCurrent();
                    SceneManager::AddScene("New Scene", new Scene());
                    SceneManager::LoadScene("New Scene");
                }

                if (ImGui::MenuItem("Load Scene", "Ctrl+L", &_openLoadSceneWindow)) {
                    //ImGui::OpenPopup("Load Scene##File_Scene_Load_Internal", ImGuiPopupFlags_NoReopen);
                    _openLoadSceneWindow = true;
                }

                if (ImGui::MenuItem("Load Scene From File", "Ctrl+Shift+L", &_fileDialogSceneOpen)) {
                    _fileDialogSceneOpen = true;
                    _fileDialogSceneOpenInfo.type = ImGuiFileDialogType_OpenFile;
                    _fileDialogSceneOpenInfo.title = "Load Scene##File_Scene_Load_File";
                    _fileDialogSceneOpenInfo.directoryPath = std::filesystem::path(std::filesystem::current_path().string() + "\\res\\scenes");
                }

                if (ImGui::MenuItem("Reload Scene", "Ctrl+R")) {
                    std::string name = SceneManager::GetCurrentSceneName();
                    SceneManager::UnloadCurrent();
                    SceneManager::LoadScene(name);
                }

                if (ImGui::MenuItem("Save Scene##File", "Ctrl+S"))
                    if (SceneManager::GetCurrentScenePath() != "") SceneManager::SaveScene(SceneManager::GetCurrentScenePath());
                    else {
                        _fileDialogSceneSave = true;
                        _fileDialogSceneSaveInfo.type = ImGuiFileDialogType_SaveFile;
                        _fileDialogSceneSaveInfo.title = "Save Scene##File_Scene_Save";
                        _fileDialogSceneSaveInfo.directoryPath = std::filesystem::path(std::filesystem::current_path().string() + "\\res\\scenes");
                    }

                if (ImGui::MenuItem("Save Scene As...##File", "Ctrl+Shift+S", &_fileDialogSceneSave)) {
                    _fileDialogSceneSave = true;
                    _fileDialogSceneSaveInfo.type = ImGuiFileDialogType_SaveFile;
                    _fileDialogSceneSaveInfo.title = "Save Scene##File_Scene_Save";
                    _fileDialogSceneSaveInfo.directoryPath = std::filesystem::path(std::filesystem::current_path().string() + "\\res\\scenes");
                }

                if (ImGui::MenuItem("Exit##File", "Esc"))
                    window->Close();

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Resources##Menu"))
            {
                ImGui::MenuItem("Font Manager##Resources", NULL, &_fontOpened);
                ImGui::MenuItem("Audio Manager##Resources", NULL, &_audioOpened);
                ImGui::MenuItem("Materials Manager##Resources", NULL, &_materialsOpened);
                ImGui::MenuItem("Textures Manager##Resources", NULL, &_texturesOpened);
                ImGui::MenuItem("Sprite Manager##Resources", NULL, &_spriteOpened);
                ImGui::MenuItem("Models Manager##Resources", NULL, &_modelsOpened);
                ImGui::MenuItem("Prefab Manager##Resources", NULL, &_prefabOpened);
                ImGui::MenuItem("Scriptable Objects Manager##Resources", NULL, &_scriptableOpened);

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Windows##Menu"))
            {
                ImGui::MenuItem("Console##Resources", NULL, &_consoleOpened, USE_IMGUI_CONSOLE_OUTPUT);

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        if (_consoleOpened) {
#if USE_IMGUI_CONSOLE_OUTPUT

#if TRACY_PROFILER
            FrameMarkStart(tracy_ImGuiDrawingConsole);
#endif

            ImGuiSink<mutex>::DrawEditor(&_consoleOpened);

#if TRACY_PROFILER
            FrameMarkEnd(tracy_ImGuiDrawingConsole);
#endif

#endif
        }

        if (_fontOpened)
            FontManager::DrawEditor(&_fontOpened);

        if (_audioOpened)
            AudioManager::DrawEditor(&_audioOpened);

        if (_materialsOpened)
            MaterialsManager::DrawEditor(&_materialsOpened);

        if (_texturesOpened)
            TextureManager::DrawEditor(&_texturesOpened);

        if (_spriteOpened)
            SpriteManager::DrawEditor(&_spriteOpened);

        if (_modelsOpened)
            ModelsManager::DrawEditor(&_modelsOpened);

        if (_prefabOpened)
            PrefabManager::DrawEditor(&_prefabOpened);

        if (_scriptableOpened)
            ScriptableObjectManager::DrawEditor(&_scriptableOpened);

        if (_openLoadSceneWindow) {
            if (ImGui::Begin("Load Scene##File_Scene_Load_Internal", &_openLoadSceneWindow, ImGuiWindowFlags_NoDocking)) {

                vector<string> scenes = SceneManager::GetAllLoadedScenesNames();

                string choosed = SceneManager::GetCurrentSceneName();

                bool clicked = false;

                if (ImGui::BeginCombo("Scenes##File_Scene_Load_Internal SCENES POPUP COMBO", choosed.c_str())) {

                    for (auto& item : scenes) {

                        if (ImGui::Selectable(std::string(item).append("##File_Scene_Load_Internal SCENES POPUP COMBO").c_str(), item == choosed)) {

                            if (clicked) continue;

                            choosed = item;
                            clicked = true;
                        }
                    }

                    if (clicked) {
                        SceneManager::UnloadCurrent();
                        SceneManager::LoadScene(choosed);
                    }

                    ImGui::EndCombo();
                }

                if (clicked) {
                    _openLoadSceneWindow = false;
                }

                ImGui::End();
            }
        }

        if (ImGui::FileDialog(&_fileDialogSceneOpen, &_fileDialogSceneOpenInfo))
        {
            // Result path in: m_fileDialogInfo.resultPath
            std::string path = std::filesystem::relative(_fileDialogSceneOpenInfo.resultPath).string();
            std::string name = std::filesystem::path(path).stem().string();
            SceneManager::AddScene(name, path);
            SceneManager::UnloadCurrent();
            SceneManager::LoadScene(name);
        }

        if (ImGui::FileDialog(&_fileDialogSceneSave, &_fileDialogSceneSaveInfo))
        {
            // Result path in: m_fileDialogInfo.resultPath
            std::string path = std::filesystem::relative(_fileDialogSceneSaveInfo.resultPath).string();
            std::string name = std::filesystem::path(path).stem().string();
            SceneManager::SaveScene(path);
            SceneManager::UnloadCurrent();
            SceneManager::AddScene(name, path);
            SceneManager::LoadScene(name);
        }

        ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), "Hello World!");

        if (ImGui::CollapsingHeader("Help")) {
            ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "Press Left ALT to disable GUI and start Moving Camera");
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), "Move Camera using WASD");
            ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), "Rotate Camera using Mouse");
            ImGui::Spacing();
        }

        window->DrawEditor();

        ImGui::Separator();

#pragma region MATERIAL_CREATOR

        if (ImGui::CollapsingHeader("Material Creator"))
        {
            static char shaderName[255] = { '\0' };
            static char materialName[255] = { '\0' };
            ImGui::Text("Shader name:");
            ImGui::SameLine();
            ImGui::InputText("##selectedItem", shaderName, 254);
            ImGui::Text("Material name:");
            ImGui::SameLine();
            ImGui::InputText("##dstPath", materialName, 254);
            if (ImGui::Button("Create Material"))
            {
                Editor::Common::CreateMaterial(shaderName, materialName);
            }
        }

#pragma endregion

        ImGui::Separator();

#pragma region EDITOR_MATERIAL_SCANNING

        if (ImGui::Button("Scan for materials"))
        {
            filesystem::path src = "res/models";
            filesystem::path dst = "res/materials/processed";
            Editor::Common::processMTLFiles(src, dst);
        }


#pragma endregion

        ImGui::Separator();

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::End();
    }
}

void end_imgui()
{
    ImGui::Render();
    window->Use();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

#endif