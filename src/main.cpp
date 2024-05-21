#define VERSION "0.7"

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

// TILEMAP
#include <Tilemap/HexagonalTilemap.h>
#include <Tilemap/HexagonalTile.h>

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

// YAML CONVERTERS
#include <tools/YamlConverters.h>
#include <Generation/YamlConverters.h>

// LOGGER
#include <tools/FileLoggerSink.h>

#if _DEBUG
// EDITOR
#include <Editor/Common/MaterialCreator.h>
#include <Editor/Common/ProcessingMtlFiles.h>
#include <Editor/Common/ScriptableObjectEditorManager.h>
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
void update();

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

Window* window;

Material material;
Material material2;
Material wallMat;
Material roofMat;
InstantiatingModel modelMesh;
GameObject* gameObject;
GameObject* gameObject2;
GameObject* gameObject3;
GameObject* gameObject4;

GameObject* imageObj;
GameObject* imageObj2;
GameObject* imageObj3;
Image* image;
float colorSpan = 1.f;
Text* text;

GameObject* Camera;

GameObject* tilemapGO = nullptr;

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

    SPDLOG_INFO("Twin^2 Engine Version: %s", VERSION);

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

#pragma endregion

#pragma region GAME_SCRIPTS_COMPONENTS

    ADD_COMPONENT("PlaneGenerator", PlaneGenerator);

#pragma endregion


    SceneManager::GetOnSceneLoaded() += [](std::string sceneName) -> void {
        Camera = SceneManager::GetRootObject()->GetComponentInChildren<CameraComponent>()->GetGameObject();
        image = SceneManager::FindObjectByName("imageObj3")->GetComponent<Image>();
        text = SceneManager::FindObjectByName("textObj")->GetComponent<Text>();
    };

    // ADDING SCENES
    SceneManager::AddScene("testScene", "res/scenes/procedurallyGenerated.scene");
    //SceneManager::AddScene("testScene", "res/scenes/quickSavedScene.scene");
    //SceneManager::AddScene("testScene", "res/scenes/quickSavedScene_Copy.scene");
    //SceneManager::AddScene("testScene", "res/scenes/ToonShading.scene");

    SceneManager::LoadScene("testScene");
    SceneManager::Update();

    //SceneManager::SaveScene("res/scenes/ToonShading.scene");

#pragma region SETTING_UP_GENERATION

    tilemapGO = SceneManager::GetGameObjectWithId(14);
    //HexagonalTilemap* hexagonalTilemap = tilemapGO->GetComponent<HexagonalTilemap>();
    MapGenerator* mapGenerator = tilemapGO->GetComponent<MapGenerator>();
    //mapGenerator->tilemap = hexagonalTilemap;
    //float tilemapGenerating = glfwGetTime();
    mapGenerator->Generate();
    //spdlog::info("Tilemap generation: {}", glfwGetTime() - tilemapGenerating);

    //ContentGenerator* contentGenerator = tilemapGO->GetComponent<ContentGenerator>();

    //tilemapGenerating = glfwGetTime();
    //contentGenerator->GenerateContent(hexagonalTilemap);
    //spdlog::info("Tilemap content generation: {}", glfwGetTime() - tilemapGenerating);
#if _DEBUG
    Editor::Common::ScriptableObjectEditorManager::Init();
    Editor::Common::ScriptableObjectEditorManager::Update();
#endif

#pragma endregion
    
    Camera = SceneManager::GetRootObject()->GetComponentInChildren<CameraComponent>()->GetGameObject();
    image = SceneManager::FindObjectByName("imageObj3")->GetComponent<Image>();
    text = SceneManager::FindObjectByName("textObj")->GetComponent<Text>();


#pragma region GAMESCRIPTS_PREFABS
    //GameObject* gs_go = SceneManager::CreateGameObject();
    //GameScripts::MovementController* gsc = gs_go->AddComponent<GameScripts::MovementController>();
    //PrefabManager::SaveAsPrefab(gs_go, "MovementControllerPref.prefab");
#pragma endregion

#if _DEBUG
    GameEngine::LateRender += []() -> void {
        ZoneScoped;

        // Draw ImGui
        FrameMarkStart(tracy_RenderingImGui);
        begin_imgui();
        render_imgui(); // edit this function to add your own ImGui controls
        end_imgui(); // this call effectively renders ImGui    
        FrameMarkEnd(tracy_RenderingImGui);
    };

    GameEngine::EarlyEnd += []() -> void {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        Editor::Common::ScriptableObjectEditorManager::End();
    };
#endif

    GameEngine::OnInput += [&]() -> void {
        input();
    };

    GameEngine::EarlyUpdate += [&]() -> void {
        update();
    };

    

    GameEngine::Start();

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

    //if (Input::IsMouseButtonPressed(Input::GetMainWindow(), Twin2Engine::Core::MOUSE_BUTTON::LEFT)) {
    //    static int i = 1;
    //    RaycastHit raycast;
    //    Ray ray = c->GetScreenPointRay(Input::GetCursorPos());
    //    CollisionManager::Instance()->Raycast(ray, raycast);
    //    if (raycast.collider != nullptr) {
    //        SPDLOG_INFO("[Click {}].\t {}. collider:\ncolpos: \t{}\t{}\t{} \nintpos: \t{}\t{}\t{}", i++, raycast.collider->colliderId, raycast.collider->collider->shapeColliderData->Position.x, raycast.collider->collider->shapeColliderData->Position.y, raycast.collider->collider->shapeColliderData->Position.z, raycast.position.x, raycast.position.y, raycast.position.z);
    //    }
    //    else {
    //        SPDLOG_INFO("Collision not happened!");
    //    }
    //}


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

    //if (LightingController::IsInstantiated() && moved) {
    //    //glm::vec3 cp = c->GetTransform()->GetGlobalPosition();
    //    //LightingController::Instance()->SetViewerPosition(cp);
    //    LightingController::Instance()->UpdateOnTransformChange();
    //}


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

    if (Input::IsKeyDown(KEY::LEFT_CONTROL) && Input::IsKeyPressed(KEY::R)) {
        SceneManager::LoadScene("testScene");
    }

    if (Input::IsKeyDown(KEY::LEFT_CONTROL) && Input::IsKeyPressed(KEY::Q)) {
        SceneManager::SaveScene("res/scenes/quickSavedScene.yaml");
    }
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

void update()
{
    // Update game objects' state here
    text->SetText(L"FPS: " + std::to_wstring(1.f / Time::GetDeltaTime()));
    colorSpan -= Time::GetDeltaTime() * 0.2f;
    if (colorSpan <= 0.f) {
        colorSpan = 1.f;
    }
    // Color
    if (colorSpan > 0.66f) {
        image->SetColor({ 0.f, 1.f, 0.f, 1.f });
    }
    else if (colorSpan > 0.33f) {
        image->SetColor({ 1.f, 1.f, 0.f, 1.f });
    }
    else {
        image->SetColor({ 1.f, 0.f, 0.f, 1.f });
    }
    // WIDTH
    image->SetWidth(1000.f * colorSpan);
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
    ImFont* font = io.Fonts->AddFontFromFileTTF("./res/fonts/NotoSans-Regular.ttf", 18.f, nullptr, ImGui::GetGlyphRangesPolish());
    IM_ASSERT(font != NULL);
    font = io.Fonts->AddFontFromFileTTF("./res/fonts/NotoSans-Bold.ttf", 18.f, nullptr, ImGui::GetGlyphRangesPolish());
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
    ZoneScoped;
    if (Input::GetCursorState() == CURSOR_STATE::NORMAL)
    {
        SceneManager::DrawCurrentSceneEditor();

#pragma region IMGUI_LOGGING_CONSOLE

#if USE_IMGUI_CONSOLE_OUTPUT

        FrameMarkStart(tracy_ImGuiDrawingConsole);
        ImGuiSink<mutex>::Draw();
        FrameMarkEnd(tracy_ImGuiDrawingConsole);

#endif
        
#pragma endregion 

        if (!ImGui::Begin("Twin^2 Engine", NULL, ImGuiWindowFlags_MenuBar)) {
            ImGui::End();
            return;
        }

        static bool _fontOpened = false;
        static bool _audioOpened = false;
        static bool _materialsOpened = false;
        static bool _texturesOpened = false;
        static bool _spriteOpened = false;
        static bool _modelsOpened = false;
        static bool _prefabOpened = false;

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File##Menu"))
            {
                //ImGui::MenuItem("Load Scene");
                //ImGui::MenuItem("Save Scene");
                //ImGui::MenuItem("Save Scene As...");
                if (ImGui::MenuItem("Exit##File"))
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
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
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

        Editor::Common::ScriptableObjectEditorManager::Draw();

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

#pragma region IMGUI_LIGHTING_SETUP
        if (ImGui::CollapsingHeader("Lighting Setup")) {
            static uint32_t shadingType = 0;
            if (ImGui::BeginCombo("Shading Type", "Lambert + Blinn-Phong Shading")) {
                if (ImGui::Selectable("Lambert + Blinn-Phong Shading", shadingType == 0)) {
                    shadingType = 0;
                    LightingController::Instance()->SetShadingType(0);
                }
                if (ImGui::Selectable("Toon/Cel Shading", shadingType == 1)) {
                    shadingType = 1;
                    LightingController::Instance()->SetShadingType(1);
                }
                if (ImGui::Selectable("Gooch Shading", shadingType == 2)) {
                    shadingType = 2;
                    LightingController::Instance()->SetShadingType(2);
                }
                ImGui::EndCombo();
            }
            // DEFAULT MATERIAL SETTINGS
            Material defaultMat = MaterialsManager::GetMaterial("Default");

            static bool hasDiffuseTexture = defaultMat.GetMaterialParameters()->Get<bool>("has_diffuse_texture");
            if (ImGui::Checkbox("Has Diffuse Texture", &hasDiffuseTexture)) {
                if (hasDiffuseTexture != defaultMat.GetMaterialParameters()->Get<bool>("has_diffuse_texture")) {
                    defaultMat.GetMaterialParameters()->Set("has_diffuse_texture", hasDiffuseTexture);
                }
            }

            static bool hasSpecularTexture = defaultMat.GetMaterialParameters()->Get<bool>("has_specular_texture");
            if (ImGui::Checkbox("Has Specular Texture", &hasSpecularTexture)) {
                if (hasSpecularTexture != defaultMat.GetMaterialParameters()->Get<bool>("has_specular_texture")) {
                    defaultMat.GetMaterialParameters()->Set("has_specular_texture", hasSpecularTexture);
                }
            }

            static vec3 color = defaultMat.GetMaterialParameters()->Get<vec3>("color");
            if (ImGui::ColorEdit3("Color", (float*)&color)) {
                if (color != defaultMat.GetMaterialParameters()->Get<vec3>("color")) {
                    defaultMat.GetMaterialParameters()->Set("color", color);
                }
            }

            static float shininess = defaultMat.GetMaterialParameters()->Get<float>("shininess");
            if (ImGui::InputFloat("Shininess", &shininess)) {
                if (shininess != defaultMat.GetMaterialParameters()->Get<float>("shininess")) {
                    defaultMat.GetMaterialParameters()->Set("shininess", shininess);
                }
            }

            // TOON SHADING VARIABLES
            if (shadingType == 1) {
                static uint32_t diffuseToonBorders = defaultMat.GetMaterialParameters()->Get<uint32_t>("diffuse_toon_borders");
                if (ImGui::InputInt("Diffuse Borders", (int*)&diffuseToonBorders)) {
                    if (diffuseToonBorders != defaultMat.GetMaterialParameters()->Get<uint32_t>("diffuse_toon_borders")) {
                        defaultMat.GetMaterialParameters()->Set("diffuse_toon_borders", diffuseToonBorders);
                    }
                }

                static uint32_t specularToonBorders = defaultMat.GetMaterialParameters()->Get<uint32_t>("specular_toon_borders");
                if (ImGui::InputInt("Specular Borders", (int*)&specularToonBorders)) {
                    if (specularToonBorders != defaultMat.GetMaterialParameters()->Get<uint32_t>("specular_toon_borders")) {
                        defaultMat.GetMaterialParameters()->Set("specular_toon_borders", specularToonBorders);
                    }
                }

                static vec2 highlightTranslate = defaultMat.GetMaterialParameters()->Get<vec2>("highlight_translate");
                if (ImGui::InputFloat2("Highlight Translate", (float*)&highlightTranslate)) {
                    if (highlightTranslate != defaultMat.GetMaterialParameters()->Get<vec2>("highlight_translate")) {
                        defaultMat.GetMaterialParameters()->Set("highlight_translate", highlightTranslate);
                    }
                }

                static vec2 highlightRotation = defaultMat.GetMaterialParameters()->Get<vec2>("highlight_rotation");
                if (ImGui::InputFloat2("Highlight Rotation", (float*)&highlightRotation)) {
                    if (highlightRotation != defaultMat.GetMaterialParameters()->Get<vec2>("highlight_rotation")) {
                        defaultMat.GetMaterialParameters()->Set("highlight_rotation", highlightRotation);
                    }
                }

                static vec2 highlightScale = defaultMat.GetMaterialParameters()->Get<vec2>("highlight_scale");
                if (ImGui::InputFloat2("Highlight Scale", (float*)&highlightScale)) {
                    if (highlightScale != defaultMat.GetMaterialParameters()->Get<vec2>("highlight_scale")) {
                        defaultMat.GetMaterialParameters()->Set("highlight_scale", highlightScale);
                    }
                }

                static vec2 highlightSplit = defaultMat.GetMaterialParameters()->Get<vec2>("highlight_split");
                if (ImGui::InputFloat2("Highlight Split", (float*)&highlightSplit)) {
                    if (highlightSplit != defaultMat.GetMaterialParameters()->Get<vec2>("highlight_split")) {
                        defaultMat.GetMaterialParameters()->Set("highlight_split", highlightSplit);
                    }
                }

                static int highlightSquareN = defaultMat.GetMaterialParameters()->Get<int>("highlight_square_n");
                if (ImGui::InputInt("Highlight Square N", &highlightSquareN)) {
                    if (highlightSquareN != defaultMat.GetMaterialParameters()->Get<int>("highlight_square_n")) {
                        defaultMat.GetMaterialParameters()->Set("highlight_square_n", highlightSquareN);
                    }
                }

                static float highlightSquareX = defaultMat.GetMaterialParameters()->Get<float>("highlight_square_x");
                if (ImGui::InputFloat("Highlight Square X", &highlightSquareX)) {
                    if (highlightSquareX != defaultMat.GetMaterialParameters()->Get<float>("highlight_square_x")) {
                        defaultMat.GetMaterialParameters()->Set("highlight_square_x", highlightSquareX);
                    }
                }
            }
        }
#pragma endregion

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