// HID
#include <core/Input.h>

// TIME
#include <core/Time.h>

// WINDOW
#include <graphic/Window.h>

// MANAGERS
#include <graphic/manager/TextureManager.h>
#include <graphic/manager/SpriteManager.h>
#include <graphic/manager/FontManager.h>
#include <graphic/manager/ShaderManager.h>
#include <graphic/manager/MaterialsManager.h>
#include <graphic/manager/ModelsManager.h>
#include <manager/AudioManager.h>

// GAME OBJECT
#include <core/GameObject.h>
#include <core/MeshRenderer.h>

// UI
#include <ui/Image.h>
#include <ui/Text.h>
#include <ui/Button.h>

// AUDIO
#include <core/AudioComponent.h>

// GRAPHIC_ENGINE
#include <GraphicEnigineManager.h>

// LOGGER
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>
#include <string>

// COLLISIONS
#include <CollisionManager.h>
#include <core/BoxColliderComponent.h>
#include <core/CapsuleColliderComponent.h>
#include <core/SphereColliderComponent.h>

// CAMERA
#include <core/CameraComponent.h>

// SCENE
#include <core/Scene.h>
#include <manager/SceneManager.h>

// PREFABS
#include <core/Prefab.h>
#include <manager/PrefabManager.h>

// DESERIALIZATION
#include <core/ComponentDeserializer.h>

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

// LIGHTING
#include <LightingController.h>
#include <core/PointLightComponent.h>
#include <core/SpotLightComponent.h>
#include <core/DirectionalLightComponent.h>

// PROCESSES
#include <processes/SynchronizedProcess.h>
#include <processes/ThreadProcess.h>
#include <processes/TimerProcess.h>
#include <processes/ProcessManager.h>

// YAML CONVERTERS
#include <core/YamlConverters.h>
#include <Generation/YamlConverters.h>

// EDITOR
#include <Editor/Common/ProcessingMtlFiles.h>
#include <Editor/Common/MaterialCreator.h>

#include <imgui_demo.cpp>

using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::UI;
using namespace Twin2Engine::GraphicEngine;
using namespace CollisionSystem;
using namespace LightingSystem;
bool updateShadowLightingMap = true;

using Twin2Engine::Core::Input;
using Twin2Engine::Core::KEY;
using Twin2Engine::Core::MOUSE_BUTTON;
using Twin2Engine::Core::CURSOR_STATE;
using Twin2Engine::Core::Time;

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

#if _DEBUG
#pragma region OpenGLCallbackFunctions

static void glfw_error_callback(int error, const char* description)
{
    spdlog::error("Glfw Error {0}: {1}\n", error, description);
}

static void GLAPIENTRY ErrorMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    //if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return; // Chce ignorowa� notyfikacje

    string severityS = "";
    if (severity == GL_DEBUG_SEVERITY_HIGH) severityS = "HIGHT";
    else if (severity == GL_DEBUG_SEVERITY_MEDIUM) severityS = "MEDIUM";
    else if (severity == GL_DEBUG_SEVERITY_LOW) severityS = "LOW";
    else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) severityS = "NOTIFICATION";

    if (type == GL_DEBUG_TYPE_ERROR) {
        spdlog::error("GL CALLBACK: type = ERROR, severity = {0}, message = {1}\n", severityS, message);
    }
    else if (type == GL_DEBUG_TYPE_MARKER) {
        spdlog::info("GL CALLBACK: type = MARKER, severity = {0}, message = {1}\n", severityS, message);
    }
    else {
        string typeS = "";
        if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR) typeS = "DEPRACTED BEHAVIOUR";
        else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR) typeS = "UNDEFINED BEHAVIOUR";
        else if (type == GL_DEBUG_TYPE_PORTABILITY) typeS = "PORTABILITY";
        else if (type == GL_DEBUG_TYPE_PERFORMANCE) typeS = "PERFORMANCE";
        else if (type == GL_DEBUG_TYPE_PUSH_GROUP) typeS = "PUSH GROUP";
        else if (type == GL_DEBUG_TYPE_POP_GROUP) typeS = "POP GROUP";
        else if (type == GL_DEBUG_TYPE_OTHER) typeS = "OTHER";
        spdlog::warn("GL CALLBACK: type = {0}, severity = {1}, message = {2}\n", typeS, severityS, message);
    }
}

#pragma endregion
#endif

#pragma region FunctionsDeclaration

bool init();

void input();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void update();
void render();

#if _DEBUG
void init_imgui();
void imgui_begin();
void imgui_render();
void imgui_end();
#endif

void end_frame();

#pragma endregion

constexpr int32_t WINDOW_WIDTH = 1920;
constexpr int32_t WINDOW_HEIGHT = 1080;
const char* WINDOW_NAME = "Twin^2 Engine";

Window* window = nullptr;

// Change these to lower GL version like 4.5 if GL 4.6 can't be initialized on your machine
const     char* glsl_version = "#version 450";
constexpr int32_t GL_VERSION_MAJOR = 4;
constexpr int32_t GL_VERSION_MINOR = 5;

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

    if (!init())
    {
        spdlog::error("Failed to initialize project!");
        return EXIT_FAILURE;
    }
    spdlog::info("Initialized project.");

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

    // Initialize stdout color sink
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::debug);

    // Create a logger with the stdout color sink
    auto logger = std::make_shared<spdlog::logger>("logger", console_sink);
    spdlog::register_logger(logger);

    // Set global log level to debug
    spdlog::set_level(spdlog::level::debug);

    GraphicEngineManager::Init();

#pragma region DESERIALIZERS
    // COMPONENTS DESELIALIZERS
    ComponentDeserializer::AddDeserializer("Camera",
        []() -> Component* {
            return new CameraComponent();
        },
        [](Component* comp, const YAML::Node& node) -> void {
            CameraComponent* cam = static_cast<CameraComponent*>(comp);
            cam->SetFOV(node["fov"].as<float>());
            cam->SetNearPlane(node["nearPlane"].as<float>());
            cam->SetFarPlane(node["farPlane"].as<float>());
            cam->SetCameraFilter(node["cameraFilter"].as<size_t>());
            cam->SetCameraType(node["cameraType"].as<CameraType>());
            cam->SetSamples(node["samples"].as<size_t>());
            cam->SetRenderResolution(node["renderRes"].as<RenderResolution>());
            cam->SetGamma(node["gamma"].as<float>());
            cam->SetWorldUp(node["worldUp"].as<vec3>());
            cam->SetIsMain(node["isMain"].as<bool>());
        }
        );

    ComponentDeserializer::AddDeserializer("Audio",
        []() -> Component* {
            return new AudioComponent();
        },
        [](Component* comp, const YAML::Node& node) -> void {
            AudioComponent* audio = static_cast<AudioComponent*>(comp);
            audio->SetAudio(SceneManager::GetAudio(node["audio"].as<size_t>()));
            if (node["loop"].as<bool>()) audio->Loop(); else audio->UnLoop();
            audio->SetVolume(node["volume"].as<float>());
        }
        );

    ComponentDeserializer::AddDeserializer("Button",
        []() -> Component* {
            return new Button();
        },
        [](Component* comp, const YAML::Node& node) -> void {
            Button* button = static_cast<Button*>(comp);
            button->SetWidth(node["width"].as<float>());
            button->SetHeight(node["height"].as<float>());
            button->SetInteractable(node["interactable"].as<bool>());
        }
        );

    // Only for subTypes
    ComponentDeserializer::AddDeserializer("Renderable",
        []() -> Component* {
            return nullptr;
        },
        [](Component* comp, const YAML::Node& node) -> void {
            RenderableComponent* renderable = static_cast<RenderableComponent*>(comp);
            renderable->SetIsTransparent(node["isTransparent"].as<bool>());
        }
        );

    ComponentDeserializer::AddDeserializer("Image",
        []() -> Component* {
            return new Image();
        },
        [](Component* comp, const YAML::Node& node) -> void {
            Image* img = static_cast<Image*>(comp);
            img->SetSprite(SceneManager::GetSprite(node["sprite"].as<size_t>()));
            img->SetColor(node["color"].as<vec4>());
            img->SetWidth(node["width"].as<float>());
            img->SetHeight(node["height"].as<float>());
        }
        );

    ComponentDeserializer::AddDeserializer("Text",
        []() -> Component* {
            return new Text();
        },
        [](Component* comp, const YAML::Node& node) -> void {
            Text* text = static_cast<Text*>(comp);
            text->SetText(node["text"].as<string>());
            text->SetColor(node["color"].as<vec4>());
            text->SetSize(node["size"].as<uint32_t>());
            text->SetFont(SceneManager::GetFont(node["font"].as<size_t>()));
        }
        );

    ComponentDeserializer::AddDeserializer("MeshRenderer",
        []() -> Component* {
            return new MeshRenderer();
        },
        [](Component* comp, const YAML::Node& node) -> void {
            MeshRenderer* meshRenderer = static_cast<MeshRenderer*>(comp);
            for (const YAML::Node& matNode : node["materials"]) {
                meshRenderer->AddMaterial(SceneManager::GetMaterial(matNode.as<size_t>()));
            }
            meshRenderer->SetModel(SceneManager::GetModel(node["model"].as<size_t>()));
        }
        );

    // Only for subTypes
    ComponentDeserializer::AddDeserializer("Collider",
        []() -> Component* {
            return nullptr;
        },
        [](Component* comp, const YAML::Node& node) -> void {
            ColliderComponent* collider = static_cast<ColliderComponent*>(comp);
            collider->SetTrigger(node["trigger"].as<bool>());
            collider->SetStatic(node["static"].as<bool>());
            collider->SetLayer(node["layer"].as<Layer>());
            LayerCollisionFilter filter = node["layerFilter"].as<LayerCollisionFilter>();
            collider->SetLayersFilter(filter);
            collider->EnableBoundingVolume(node["boundingVolume"].as<bool>());
            collider->SetBoundingVolumeRadius(node["boundingVolumeRadius"].as<float>());
            vec3 position = node["position"].as<vec3>();
            collider->SetLocalPosition(position.x, position.y, position.z);
        }
        );

    ComponentDeserializer::AddDeserializer("SphereCollider",
        []() -> Component* {
            return new SphereColliderComponent();
        },
        [](Component* comp, const YAML::Node& node) -> void {
            SphereColliderComponent* sphereCollider = static_cast<SphereColliderComponent*>(comp);
            sphereCollider->SetRadius(node["radius"].as<float>());
        }
        );

    ComponentDeserializer::AddDeserializer("BoxCollider",
        []() -> Component* {
            return new BoxColliderComponent();
        },
        [](Component* comp, const YAML::Node& node) -> void {
            BoxColliderComponent* boxCollider = static_cast<BoxColliderComponent*>(comp);
            boxCollider->SetWidth(node["width"].as<float>());
            boxCollider->SetLength(node["length"].as<float>());
            boxCollider->SetHeight(node["height"].as<float>());
            vec3 rotation = node["rotation"].as<vec3>();
            boxCollider->SetXRotation(rotation.x);
            boxCollider->SetYRotation(rotation.y);
            boxCollider->SetZRotation(rotation.z);
        }
        );

    ComponentDeserializer::AddDeserializer("CapsuleCollider",
        []() -> Component* {
            return new CapsuleColliderComponent();
        },
        [](Component* comp, const YAML::Node& node) -> void {
            CapsuleColliderComponent* capsuleCollider = static_cast<CapsuleColliderComponent*>(comp);
            vec3 endPos = node["endPosition"].as<vec3>();
            capsuleCollider->SetEndPosition(endPos.x, endPos.y, endPos.z);
            capsuleCollider->SetRadius(node["radius"].as<float>());
        }
        );

    ComponentDeserializer::AddDeserializer("LightComponent",
        []() -> Component* {
            return nullptr;
        },
        [](Component* comp, const YAML::Node& node) -> void {
            //LightingSystem::LightComponent* lightComponent = static_cast<LightingSystem::LightComponent*>(comp);
        }
        );

    ComponentDeserializer::AddDeserializer("DirectionalLightComponent",
        []() -> Component* {
            return new DirectionalLightComponent();
        },
        [](Component* comp, const YAML::Node& node) -> void {
            DirectionalLightComponent* light = static_cast<DirectionalLightComponent*>(comp);
            light->SetDirection(node["direction"].as<vec3>());
            light->SetColor(node["color"].as<vec3>());
            light->SetPower(node["power"].as<float>());
        }
        );

    ComponentDeserializer::AddDeserializer("PointLightComponent",
        []() -> Component* {
            return new PointLightComponent();
        },
        [](Component* comp, const YAML::Node& node) -> void {
            PointLightComponent* light = static_cast<PointLightComponent*>(comp);
            light->SetColor(node["color"].as<vec3>());
            light->SetPower(node["power"].as<float>());
            light->SetAttenuation(node["constant"].as<float>(), node["linear"].as<float>(), node["quadratic"].as<float>());
        }
        );

    ComponentDeserializer::AddDeserializer("SpotLightComponent",
        []() -> Component* {
            return new SpotLightComponent();
        },
        [](Component* comp, const YAML::Node& node) -> void {
            SpotLightComponent* light = static_cast<SpotLightComponent*>(comp);
            light->SetDirection(node["direction"].as<vec3>());
            light->SetColor(node["color"].as<vec3>());
            light->SetPower(node["power"].as<float>());
            light->SetOuterCutOff(node["outerCutOff"].as<float>());
            light->SetAttenuation(node["constant"].as<float>(), node["linear"].as<float>(), node["quadratic"].as<float>());
        }
        );

#pragma endregion

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

    // ADDING SCENES
    //SceneManager::AddScene("testScene", "res/scenes/quickSavedScene.yaml");
    SceneManager::AddScene("testScene", "res/scenes/procedurallyGenerated.yaml");
    //SceneManager::AddScene("testScene", "res/scenes/quickSavedScene_toonShading.yaml");

    CollisionSystem::CollisionManager::Instance()->PerformCollisions();
    
    SceneManager::LoadScene("testScene");

#pragma region SETTING_UP_GENERATION

    tilemapGO = SceneManager::GetGameObjectWithId(14);
    HexagonalTilemap* hexagonalTilemap = tilemapGO->GetComponent<HexagonalTilemap>();
    MapGenerator* mapGenerator = tilemapGO->GetComponent<MapGenerator>();
    mapGenerator->tilemap = hexagonalTilemap;
    float tilemapGenerating = glfwGetTime();
    mapGenerator->Generate();
    spdlog::info("Tilemap generation: {}", glfwGetTime() - tilemapGenerating);

    ContentGenerator* contentGenerator = tilemapGO->GetComponent<ContentGenerator>();

    tilemapGenerating = glfwGetTime();
    contentGenerator->GenerateContent(hexagonalTilemap);
    spdlog::info("Tilemap content generation: {}", glfwGetTime() - tilemapGenerating);

#pragma endregion

    //InstantiatingModel modelHexagon = ModelsManager::LoadModel("res/models/hexagon.obj");
    //GameObject* hexagonPrefab = new GameObject();
    //hexagonPrefab->GetTransform()->Translate(glm::vec3(2, 3, 0));
    //hexagonPrefab->GetTransform()->SetLocalRotation(glm::vec3(0, 90, 0));
    //auto comp = hexagonPrefab->AddComponent<MeshRenderer>();
    //comp->AddMaterial(MaterialsManager::GetMaterial("multiTexture"));
    ////comp->AddMaterial(MaterialsManager::GetMaterial("RedHexTile"));
    //comp->SetModel(modelHexagon);

    Camera = SceneManager::GetRootObject()->GetComponentInChildren<CameraComponent>()->GetGameObject();
    image = SceneManager::FindObjectByName("imageObj3")->GetComponent<Image>();
    text = SceneManager::FindObjectByName("textObj")->GetComponent<Text>();

#pragma region TestingLighting
    GameObject* dl_go = SceneManager::CreateGameObject();
    dl_go->GetTransform()->SetLocalPosition(glm::vec3(10.0f, 10.0f, 0.0f));
    DirectionalLightComponent* dl = dl_go->AddComponent<DirectionalLightComponent>();
    dl->SetColor(glm::vec3(1.0f));
    //LightingController::Instance()->SetViewerPosition(cameraPos);
    LightingController::Instance()->SetAmbientLight(glm::vec3(0.1f));
#pragma endregion

    bool open = true;

    // Main loop
    while (!window->IsClosed())
    {
        // Process I/O operations here
        input();

        // Update game objects' state here
        update();

        // OpenGL rendering code here
        render();

#if _DEBUG
        // Draw ImGui
        imgui_begin();
        imgui_render(); // edit this function to add your own ImGui controls
        imgui_end(); // this call effectively renders ImGui
#endif

        // End frame and swap buffers (double buffering)
        end_frame();
    }

    // Cleanup
    SceneManager::UnloadAll();
    SpriteManager::UnloadAll();
    TextureManager::UnloadAll();
    AudioManager::UnloadAll();
    FontManager::UnloadAll();
    CollisionManager::UnloadAll();
    LightingController::UnloadAll();
    GraphicEngineManager::End();
    Input::FreeAllWindows();

#if _DEBUG
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
#endif

    Window::FreeAll();

    glfwTerminate();

    return 0;
}

bool init()
{
    // Setup window
#if _DEBUG
    glfwSetErrorCallback(glfw_error_callback);
#endif

    if (!glfwInit())
    {
        spdlog::error("Failed to initalize GLFW!");
        return false;
    }

    // GL 4.5 + GLSL 450
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    window = Window::MakeWindow(WINDOW_NAME, { WINDOW_WIDTH, WINDOW_HEIGHT }, false);
    Input::InitForWindow(window);

    bool err = !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (err)
    {
        spdlog::error("Failed to initialize OpenGL loader!");
        return false;
    }
    spdlog::info("Successfully initialized OpenGL loader!");


#ifdef _DEBUG
    // Debugging
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(ErrorMessageCallback, 0);

    const GLubyte* renderer = glGetString(GL_RENDERER);
    spdlog::info("Graphic Card: {0}", (char*)renderer);
#endif

    // Blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Depth Test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Face Culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);

    ScriptableObject::Init();

    return true;
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

    if (LightingSystem::LightingController::IsInstantiated() && moved) {
        //glm::vec3 cp = c->GetTransform()->GetGlobalPosition();
        //LightingSystem::LightingController::Instance()->SetViewerPosition(cp);
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

    if (Input::IsKeyPressed(KEY::R)) {
        SceneManager::LoadScene("testScene");
        Camera = SceneManager::GetRootObject()->GetComponentInChildren<CameraComponent>()->GetGameObject();
        image = SceneManager::FindObjectByName("imageObj3")->GetComponent<Image>();
        text = SceneManager::FindObjectByName("textObj")->GetComponent<Text>();
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
    //LightingSystem::LightingController::Instance()->UpdateOnTransformChange();
    updateShadowLightingMap = true;
}

void update()
{
    //Update Shadow & Lighting Map
    if (updateShadowLightingMap)
    {
        LightingSystem::LightingController::Instance()->UpdateOnTransformChange();
        updateShadowLightingMap = false;
    }

    // Update game objects' state here
    text->SetText("Time: " + std::to_string(Time::GetDeltaTime()));
    CollisionManager::Instance()->PerformCollisions();
    SceneManager::UpdateCurrentScene();
    Twin2Engine::Processes::ProcessManager::Instance()->UpdateSynchronizedProcess();
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

void render()
{
    // OpenGL Rendering code goes here
    SceneManager::RenderCurrentScene();
    CameraComponent::GetMainCamera()->Render();
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

    ImGui_ImplGlfw_InitForOpenGL(window->GetWindow(), true);
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

void imgui_begin()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

struct HierarchicalItem {
    std::string label;
    std::vector<HierarchicalItem> children;
};
// Recursive function to render the hierarchical list
void renderHierarchicalList(const HierarchicalItem& item) {
    if (ImGui::TreeNode(item.label.c_str())) {
        for (const auto& child : item.children) {
            renderHierarchicalList(child);
        }
        ImGui::TreePop();
    }
}
void imgui_render()
{
    if (Input::GetCursorState() == NORMAL)
    {
        SceneManager::DrawCurrentSceneEditor();

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
                    LightingSystem::LightingController::Instance()->SetShadingType(0);
                }
                if (ImGui::Selectable("Toon/Cel Shading", shadingType == 1)) {
                    shadingType = 1;
                    LightingSystem::LightingController::Instance()->SetShadingType(1);
                }
                if (ImGui::Selectable("Gooch Shading", shadingType == 2)) {
                    shadingType = 2;
                    LightingSystem::LightingController::Instance()->SetShadingType(2);
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

#pragma region ScriptableObjects

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
#pragma region MapGenerators

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

void imgui_end()
{
    ImGui::Render();
    window->Use();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

#endif

void end_frame()
{
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    Time::Update();
    Input::Update();
    window->Update();
}