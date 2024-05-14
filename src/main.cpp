#define USE_IMGUI_CONSOLE_OUTPUT true
#define USE_WINDOWS_CONSOLE_OUTPUT false

#if USE_IMGUI_CONSOLE_OUTPUT || !USE_WINDOWS_CONSOLE_OUTPUT

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#endif

#include <GameEngine.h>

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

// EDITOR
#include <Editor/Common/MaterialCreator.h>
#include <Editor/Common/ProcessingMtlFiles.h>
#include <Editor/Common/ScriptableObjectEditorManager.h>


// Przeniesc do Impl_imgui
#include <Editor/Common/ImGuiSink.h>
using Editor::Common::ImGuiSink;
using Editor::Common::ImGuiLogMessage;

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

float fmapf(float input, float currStart, float currEnd, float expectedStart, float expectedEnd);
double mod(double val1, double val2);

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
#if USE_IMGUI_CONSOLE_OUTPUT || USE_WINDOWS_CONSOLE_OUTPUT

#if USE_IMGUI_CONSOLE_OUTPUT
    auto console_sink = std::make_shared<Editor::Common::ImGuiSink<mutex>>("res/logs/log.txt");
#elif USE_WINDOWS_CONSOLE_OUTPUT
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
#endif

    auto logger = std::make_shared<spdlog::logger>("logger", console_sink);
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);

#else
    spdlog::set_level(spdlog::level::off);
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

    ComponentDeserializer::AddDeserializer("HexagonalTilemap",
        []() -> Component* {
            return new HexagonalTilemap();
        },
        [](Component* comp, const YAML::Node& node) -> void {
            HexagonalTilemap* hexagonalTilemap = static_cast<HexagonalTilemap*>(comp);
            hexagonalTilemap->Resize(node["leftBottomPosition"].as<ivec2>(), node["rightTopPosition"].as<ivec2>());

            // tilemap
        }
        );

#pragma endregion

#pragma region GENERATION_DESERIALIZER

    ComponentDeserializer::AddDeserializer("MapGenerator",
        []() -> Component* {
            return new MapGenerator();
        },
        [](Component* comp, const YAML::Node& node) -> void {
            MapGenerator* mapGenerator = static_cast<MapGenerator*>(comp);

            //Tilemap::HexagonalTilemap* tilemap;
            ////Tilemap::HexagonalTile* tile;
            //Twin2Engine::Core::GameObject* preafabHexagonalTile;
            //Twin2Engine::Core::GameObject* additionalTile;
            //Twin2Engine::Core::GameObject* filledTile;
            //Twin2Engine::Core::GameObject* pointTile;

            mapGenerator->preafabHexagonalTile = PrefabManager::LoadPrefab(node["preafabHexagonalTile"].as<string>());
            mapGenerator->additionalTile = PrefabManager::LoadPrefab(node["additionalTile"].as<string>());
            mapGenerator->filledTile = PrefabManager::LoadPrefab(node["filledTile"].as<string>());
            mapGenerator->pointTile = PrefabManager::LoadPrefab(node["pointTile"].as<string>());

            mapGenerator->generationRadiusMin = node["generationRadiusMin"].as<float>();
            mapGenerator->generationRadiusMax = node["generationRadiusMax"].as<float>();
            mapGenerator->minPointsNumber = node["minPointsNumber"].as<int>();
            mapGenerator->maxPointsNumber = node["maxPointsNumber"].as<int>();
            mapGenerator->angleDeltaRange = node["angleDeltaRange"].as<float>();
        }
        );

    ComponentDeserializer::AddDeserializer("ContentGenerator",
        []() -> Component* {
            return new ContentGenerator();
        },
        [](Component* comp, const YAML::Node& node) -> void {
            ContentGenerator* contentGenerator = static_cast<ContentGenerator*>(comp);

            for (YAML::Node soSceneId : node["mapElementGenerators"])
            {
                //AMapElementGenerator* generator = dynamic_cast<AMapElementGenerator*>(ScriptableObjectManager::Deserialize(soSceneId.as<unsigned int>()));
                AMapElementGenerator* generator = dynamic_cast<AMapElementGenerator*>(ScriptableObjectManager::Load(soSceneId.as<string>()));
                SPDLOG_INFO("Adding generator {0}, {1}", soSceneId.as<string>(), (unsigned int) generator);
                if (generator != nullptr)
                {
                    contentGenerator->mapElementGenerators.push_back(generator);
                }
            }
            //contentGenerator->mapElementGenerators = node["mapElementGenerators"].as<std::list<Generators::AMapElementGenerator*>>();
        }
        );

    ComponentDeserializer::AddDeserializer("MapHexTile",
        []() -> Component* {
            return new MapHexTile();
        },
        [](Component* comp, const YAML::Node& node) -> void {
            MapHexTile* mapHexTile = static_cast<MapHexTile*>(comp);

            mapHexTile->tilemap = nullptr;
            mapHexTile->region = nullptr;
            mapHexTile->sector = nullptr;
            mapHexTile->tile = nullptr;
            mapHexTile->type = node["hexTileType"].as<MapHexTile::HexTileType>();
            //contentGenerator->mapElementGenerators = node["mapElementGenerators"].as<std::list<Generators::AMapElementGenerator*>>();
        }
        );

    ComponentDeserializer::AddDeserializer("MapRegion",
        []() -> Component* {
            return new MapRegion();
        },
        [](Component* comp, const YAML::Node& node) -> void {
            MapRegion* mapRegion = static_cast<MapRegion*>(comp);

            mapRegion->tilemap = nullptr;
            mapRegion->type = node["regionType"].as<MapRegion::RegionType>();
        }
        );

    ComponentDeserializer::AddDeserializer("MapSector",
        []() -> Component* {
            return new MapSector();
        },
        [](Component* comp, const YAML::Node& node) -> void {
            MapSector* mapSector = static_cast<MapSector*>(comp);


            mapSector->tilemap = nullptr;
            mapSector->region = nullptr;
            mapSector->type = node["sectorType"].as<MapSector::SectorType>();
        }
        );

#pragma endregion

    SceneManager::GetOnSceneLoaded() += [](std::string sceneName) -> void {
        Camera = SceneManager::GetRootObject()->GetComponentInChildren<CameraComponent>()->GetGameObject();
        image = SceneManager::FindObjectByName("imageObj3")->GetComponent<Image>();
        text = SceneManager::FindObjectByName("textObj")->GetComponent<Text>();
    };

    // ADDING SCENES
    //SceneManager::AddScene("testScene", "res/scenes/quickSavedScene_Copy.scene");
    //SceneManager::AddScene("testScene", "res/scenes/quickSavedScene.scene");
    //SceneManager::AddScene("testScene", "res/scenes/procedurallyGenerated.scene");
    SceneManager::AddScene("testScene", "res/scenes/quickSavedScene_toonShading.scene");
    //SceneManager::AddScene("testScene", "res/scenes/DirLightTest.scene");

    SceneManager::LoadScene("testScene");
    SceneManager::Update();

    GameObject* obj = SceneManager::CreateGameObject();
    obj->SetName("Test Button");
    Transform* tr = obj->GetTransform();
    tr->Rotate(glm::vec3(0, 0, 45.f));
    tr->Translate(glm::vec3(0.f, -200.f, 0.f));
    Button* b = obj->AddComponent<Button>();
    b->SetHeight(70);
    b->SetWidth(200);
    b->GetOnClickEvent() += []() -> void {
        spdlog::info("clicked");
    };
    Image* i = obj->AddComponent<Image>();
    i->SetSprite("white_box");
    i->SetHeight(70);
    i->SetWidth(200);
    Text* t = obj->AddComponent<Text>();
    t->SetText(L"ClickMeeejjjjjjjjj");
    t->SetFont("res/fonts/Caveat-Regular.ttf");
    t->SetSize(48);  
    t->EnableAutoSize(10, 60);
    t->SetHeight(44);
    t->SetWidth(196);
    t->SetColor(glm::vec4(1.f, 0.f, 0.f, 1.f));

    obj = SceneManager::CreateGameObject();
    obj->SetName("Test Input Field");
    Image* img = obj->AddComponent<Image>();
    img->SetSprite("white_box");
    img->SetWidth(200);
    img->SetHeight(70);
    Text* inputText = obj->AddComponent<Text>();
    inputText->SetFont("res/fonts/Caveat-Regular.ttf");
    inputText->SetSize(48);
    inputText->SetWidth(196);
    inputText->SetHeight(66);
    inputText->SetTextOverflow(TextOverflow::Truncate);
    inputText->EnableAutoSize(30, 48);
    Text* placeHolder = obj->AddComponent<Text>();
    placeHolder->SetFont("res/fonts/Caveat-Regular.ttf");
    placeHolder->SetSize(48);
    placeHolder->SetWidth(196);
    placeHolder->SetHeight(66);
    placeHolder->SetText(L"Enter name...");
    placeHolder->SetTextOverflow(TextOverflow::Ellipsis);
    placeHolder->EnableAutoSize(30, 48);
    placeHolder->SetColor({ .5f, .5f, .5f, 1.f });
    InputField* inp = obj->AddComponent<InputField>();
    inp->SetInputText(inputText);
    inp->SetPlaceHolderText(placeHolder);
    inp->SetWidth(200);
    inp->SetHeight(70);

    //SceneManager::SaveScene("res/scenes/quickSavedScene_toonShading.yaml");

#pragma region SETTING_UP_GENERATION

    tilemapGO = SceneManager::GetGameObjectWithId(14);
    HexagonalTilemap* hexagonalTilemap = tilemapGO->GetComponent<HexagonalTilemap>();
    MapGenerator* mapGenerator = tilemapGO->GetComponent<MapGenerator>();
    mapGenerator->tilemap = hexagonalTilemap;
    float tilemapGenerating = glfwGetTime();
    //mapGenerator->Generate();
    spdlog::info("Tilemap generation: {}", glfwGetTime() - tilemapGenerating);

    ContentGenerator* contentGenerator = tilemapGO->GetComponent<ContentGenerator>();

    tilemapGenerating = glfwGetTime();
    //contentGenerator->GenerateContent(hexagonalTilemap);
    spdlog::info("Tilemap content generation: {}", glfwGetTime() - tilemapGenerating);
    Editor::Common::ScriptableObjectEditorManager::Update();

#pragma endregion
    
    Camera = SceneManager::GetRootObject()->GetComponentInChildren<CameraComponent>()->GetGameObject();
    image = SceneManager::FindObjectByName("imageObj3")->GetComponent<Image>();
    text = SceneManager::FindObjectByName("textObj")->GetComponent<Text>();

#pragma region TestingLighting
    /*GameObject* dl_go = SceneManager::CreateGameObject();
    dl_go->GetTransform()->SetLocalPosition(glm::vec3(10.0f, 10.0f, 0.0f));
    DirectionalLightComponent* dl = dl_go->AddComponent<DirectionalLightComponent>();
    dl->SetColor(glm::vec3(1.0f));
    //LightingController::Instance()->SetViewerPosition(cameraPos);
    LightingController::Instance()->SetAmbientLight(glm::vec3(0.1f));

    SceneManager::SaveScene("res/scenes/DirLightTest.scene");*/
#pragma endregion

#if _DEBUG
    GameEngine::LateRender += []() -> void {
        // Draw ImGui
        begin_imgui();
        render_imgui(); // edit this function to add your own ImGui controls
        end_imgui(); // this call effectively renders ImGui        
    };

    GameEngine::EarlyEnd += []() -> void {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    };
#endif

    GameEngine::OnInput += [&]() -> void {
        input();
    };

    GameEngine::EarlyUpdate += [&]() -> void {
        update();
    };

    GameEngine::Start();
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

    bool moved = false;

    if (Input::IsKeyDown(KEY::W) && Input::GetCursorState() == CURSOR_STATE::DISABLED)
    {
        Camera->GetTransform()->SetGlobalPosition(Camera->GetTransform()->GetGlobalPosition() + c->GetFrontDir() * cameraSpeed * Time::GetDeltaTime());
        moved = true;
    }
    if (Input::IsKeyDown(KEY::S) && Input::GetCursorState() == CURSOR_STATE::DISABLED)
    {
        Camera->GetTransform()->SetGlobalPosition(Camera->GetTransform()->GetGlobalPosition() - c->GetFrontDir() * cameraSpeed * Time::GetDeltaTime());
        moved = true;
    }
    if (Input::IsKeyDown(KEY::A) && Input::GetCursorState() == CURSOR_STATE::DISABLED)
    {
        Camera->GetTransform()->SetGlobalPosition(Camera->GetTransform()->GetGlobalPosition() - c->GetRight() * cameraSpeed * Time::GetDeltaTime());
        moved = true;
    }
    if (Input::IsKeyDown(KEY::D) && Input::GetCursorState() == CURSOR_STATE::DISABLED)
    {
        Camera->GetTransform()->SetGlobalPosition(Camera->GetTransform()->GetGlobalPosition() + c->GetRight() * cameraSpeed * Time::GetDeltaTime());
        moved = true;
    }

    if (LightingController::IsInstantiated() && moved) {
        //glm::vec3 cp = c->GetTransform()->GetGlobalPosition();
        //LightingController::Instance()->SetViewerPosition(cp);
        LightingController::Instance()->UpdateOnTransformChange();
    }


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
}

void update()
{
    // Update game objects' state here
    text->SetText(L"Time: " + std::to_wstring(Time::GetDeltaTime()));
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
struct HierarchicalItem {
    std::string label;
    std::vector<HierarchicalItem> children;
};
// Recursive function to render the hierarchical list
static void renderHierarchicalList(const HierarchicalItem& item) {
    if (ImGui::TreeNode(item.label.c_str())) {
        for (const auto& child : item.children) {
            renderHierarchicalList(child);
        }
        ImGui::TreePop();
    }
}

void init_imgui()
{
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls

    ImGui_ImplGlfw_InitForOpenGL(Window::GetInstance()->GetWindow(), true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);
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
    if (Input::GetCursorState() == CURSOR_STATE::NORMAL)
    {
        SceneManager::DrawCurrentSceneEditor();

#pragma region IMGUI_LOGGING_CONSOLE

#if USE_IMGUI_CONSOLE_OUTPUT
        ImGuiSink<mutex>::Draw();
#endif
        
#pragma endregion 

        if (!ImGui::Begin("Twin^2 Engine", NULL, ImGuiWindowFlags_MenuBar)) {
            ImGui::End();
            return;
        }

        static bool _fontOpened = false;
        static bool _audioOpened = false;
        static bool _materialsOpened = false;

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

#pragma region IMGUI_SCRIPTABLE_OBJECTS

        if (ImGui::CollapsingHeader("Scriptable Object Creator")) {
            static string selectedSO = "";
            static vector<string> scriptableObjectsNames = ScriptableObjectManager::GetScriptableObjectsNames();
            if (ImGui::TreeNode("ScriptableObjects")) {
                for (size_t i = 0; i < scriptableObjectsNames.size(); i++)
                {
                    if (ImGui::Selectable(scriptableObjectsNames[i].c_str())) {
                        selectedSO = scriptableObjectsNames[i];
                    }
                }
                ImGui::TreePop();
            }
            ImGui::Text("Selected Scriptable Object to create:");
            ImGui::SameLine();
            ImGui::InputText("##selectedItem", &selectedSO[0], selectedSO.size(), ImGuiInputTextFlags_ReadOnly);
            static char dstPath[255] = { '\0' };
            ImGui::Text("Destination and output file name:");
            ImGui::SameLine();
            ImGui::InputText("##dstPath", dstPath, 254);

            if (ImGui::Button("Create ScriptableObject"))
            {
                if (selectedSO.size() > 0)
                {
                    string strDstPath(dstPath);
                    if (strDstPath.size() > 0)
                    {
                        ScriptableObjectManager::CreateScriptableObject(strDstPath, selectedSO);
                    }
                }
            }
        }
#pragma endregion

#pragma region IMGUI_TEXT_TEST
        if (ImGui::CollapsingHeader("Text test")) {
            Text* t = SceneManager::FindObjectByName("Test Button")->GetComponent<Text>();
            static std::string alignYValue = t->GetTextAlignY() == TextAlignY::CENTER ? "CENTER" : t->GetTextAlignY() == TextAlignY::TOP ? "TOP" : "BOTTOM";
            if (ImGui::BeginCombo("Align Y", alignYValue.c_str())) {
                TextAlignY alignY = t->GetTextAlignY();
                if (ImGui::Selectable("BOTTOM")) {
                    if (alignY != TextAlignY::BOTTOM) {
                        t->SetTextAlignY(TextAlignY::BOTTOM);
                        alignYValue = "BOTTOM";
                    }
                }
                if (ImGui::Selectable("CENTER")) {
                    if (alignY != TextAlignY::CENTER) {
                        t->SetTextAlignY(TextAlignY::CENTER);
                        alignYValue = "CENTER";
                    }
                }
                if (ImGui::Selectable("TOP")) {
                    if (alignY != TextAlignY::TOP) {
                        t->SetTextAlignY(TextAlignY::TOP);
                        alignYValue = "TOP";
                    }
                }
                ImGui::EndCombo();
            }
            static std::string alignXValue = t->GetTextAlignX() == TextAlignX::CENTER ? "CENTER" : t->GetTextAlignX() == TextAlignX::LEFT ? "LEFT" : "RIGHT";
            if (ImGui::BeginCombo("Align X", alignXValue.c_str())) {
                TextAlignX alignX = t->GetTextAlignX();
                if (ImGui::Selectable("LEFT")) {
                    if (alignX != TextAlignX::LEFT) {
                        t->SetTextAlignX(TextAlignX::LEFT);
                        alignXValue = "LEFT";
                    }
                }
                if (ImGui::Selectable("CENTER")) {
                    if (alignX != TextAlignX::CENTER) {
                        t->SetTextAlignX(TextAlignX::CENTER);
                        alignXValue = "CENTER";
                    }
                }
                if (ImGui::Selectable("RIGHT")) {
                    if (alignX != TextAlignX::RIGHT) {
                        t->SetTextAlignX(TextAlignX::RIGHT);
                        alignXValue = "RIGHT";
                    }
                }
                ImGui::EndCombo();
            }
            static std::string overflowValue = t->GetTextOverflow() == TextOverflow::Overflow ? "OVERFLOW" : t->GetTextOverflow() == TextOverflow::Ellipsis ? "ELLIPSIS" : t->GetTextOverflow() == TextOverflow::Masking ? "MASKING" : "TRUNCATE";
            if (ImGui::BeginCombo("Overflow", overflowValue.c_str())) {
                TextOverflow overflow = t->GetTextOverflow();
                if (ImGui::Selectable("OVERFLOW")) {
                    if (overflow != TextOverflow::Overflow) {
                        t->SetTextOverflow(TextOverflow::Overflow);
                        overflowValue = "OVERFLOW";
                    }
                }
                if (ImGui::Selectable("ELLIPSIS")) {
                    if (overflow != TextOverflow::Ellipsis) {
                        t->SetTextOverflow(TextOverflow::Ellipsis);
                        overflowValue = "ELLIPSIS";
                    }
                }
                if (ImGui::Selectable("MASKING")) {
                    if (overflow != TextOverflow::Masking) {
                        t->SetTextOverflow(TextOverflow::Masking);
                        overflowValue = "MASKING";
                    }
                }
                if (ImGui::Selectable("TRUNCATE")) {
                    if (overflow != TextOverflow::Truncate) {
                        t->SetTextOverflow(TextOverflow::Truncate);
                        overflowValue = "TRUNCATE";
                    }
                }
                ImGui::EndCombo();
            }
            bool wrapping = t->IsTextWrapping();
            if (ImGui::Checkbox("Text Wrapping", &wrapping)) {
                if (wrapping != t->IsTextWrapping()) {
                    t->SetTextWrapping(wrapping);
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

        /**/
#pragma region IMGUI_MAP_GENERATOR

        if (ImGui::CollapsingHeader("Map Generator"))
        {
            static string selectedSO = "";
            static vector<string> scriptableObjectsPaths = ScriptableObjectManager::GetAllPaths();
            static ScriptableObject* selectedScriptableObject = nullptr;
            if (ImGui::TreeNode("Scriptable Objects")) {
                for (size_t i = 0; i < scriptableObjectsPaths.size(); i++)
                {
                    if (ImGui::Selectable(scriptableObjectsPaths[i].c_str())) {
                        selectedSO = scriptableObjectsPaths[i];
                        selectedScriptableObject = ScriptableObjectManager::Get(selectedSO);
                    }
                }
                ImGui::TreePop();
            }
            ImGui::Text("Selected Scriptable Object to create:");
            ImGui::SameLine();
            ImGui::InputText("##selectedSO", &selectedSO[0], selectedSO.size(), ImGuiInputTextFlags_ReadOnly);

            ImGui::Separator();
            if (selectedScriptableObject != nullptr)
            {
                selectedScriptableObject->DrawEditor();
                ImGui::Separator();
            }

            if (ImGui::Button("Generate"))
            {
                static Transform* tilemapTransform = tilemapGO->GetTransform();
                static HexagonalTilemap* hexagonalTilemap = tilemapGO->GetComponent<HexagonalTilemap>();
                hexagonalTilemap->Clear();
                while (tilemapTransform->GetChildCount())
                {
                    Transform* child = tilemapTransform->GetChildAt(0ull);
                    tilemapTransform->RemoveChild(child);
                    SceneManager::DestroyGameObject(child->GetGameObject());
                }
                
                static MapGenerator* mapGenerator = tilemapGO->GetComponent<MapGenerator>();
                
                float tilemapGenerating = glfwGetTime();
                mapGenerator->Generate();
                spdlog::info("Tilemap generation: {}", glfwGetTime() - tilemapGenerating);
                
                static ContentGenerator* contentGenerator = tilemapGO->GetComponent<ContentGenerator>();
                
                tilemapGenerating = glfwGetTime();
                contentGenerator->GenerateContent(hexagonalTilemap);
                spdlog::info("Tilemap content generation: {}", glfwGetTime() - tilemapGenerating);
            }
        }

#pragma endregion
        
        ImGui::Separator();
        /**/

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
