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

// YAML CONVERTERS
#include <core/YamlConverters.h>
#include <Generation/YamlConverters.h>

using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::UI;
using namespace Twin2Engine::GraphicEngine;
using namespace CollisionSystem;
using namespace LightingSystem;

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

float fmapf(float input, float currStart, float currEnd, float expectedStart, float expectedEnd);
double mod(double val1, double val2);

#pragma endregion

constexpr int32_t WINDOW_WIDTH  = 1920;
constexpr int32_t WINDOW_HEIGHT = 1080;
const char* WINDOW_NAME = "Twin^2 Engine";

Window* window = nullptr;

// Change these to lower GL version like 4.5 if GL 4.6 can't be initialized on your machine
const     char*   glsl_version     = "#version 450";
constexpr int32_t GL_VERSION_MAJOR = 4;
constexpr int32_t GL_VERSION_MINOR = 5;

Material material;
Material material2;
Material wallMat;
Material roofMat;
InstatiatingModel modelMesh;
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
            cam->SetFrontDir(node["frontDir"].as<vec3>());
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
                AMapElementGenerator* generator = dynamic_cast<AMapElementGenerator*>(ScriptableObjectManager::Deserialize(soSceneId.as<unsigned int>()));
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
            mapHexTile->type = node["type"].as<MapHexTile::HexTileType>();
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
            mapRegion->type = node["type"].as<MapRegion::RegionType>();
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
            mapSector->type = node["type"].as<MapSector::SectorType>();
        }
    );

#pragma endregion
    // ADDING SCENES
    //SceneManager::AddScene("testScene", "res/scenes/savedScene.yaml");
    SceneManager::AddScene("testScene", "res/scenes/quickSavedScene.yaml");
    //SceneManager::AddScene("testScene", "res/scenes/testScene.yaml");

#pragma region SETTING_UP_GENERATION
    InstatiatingModel modelHexagon = ModelsManager::LoadModel("res/models/hexagon.obj");
    GameObject* hexagonPrefab = new GameObject();
    hexagonPrefab->GetTransform()->Translate(glm::vec3(2, 3, 0));
    hexagonPrefab->GetTransform()->SetLocalRotation(glm::vec3(0, 90, 0));
    MapHexTile* mapHexTile = hexagonPrefab->AddComponent<MapHexTile>();
    
    auto comp = hexagonPrefab->AddComponent<MeshRenderer>();
    comp->AddMaterial(MaterialsManager::GetMaterial("ColoredHexTile"));
    //comp->AddMaterial(MaterialsManager::GetMaterial("RedHexTile"));
    comp->SetModel(modelHexagon);

    PrefabManager::SaveAsPrefab(hexagonPrefab, "res/prefabs/tilemap/tiles/hexTile.prefab");

    GameObject* redHexagonPrefab = new GameObject();
    redHexagonPrefab->GetTransform()->Translate(glm::vec3(3, 4, 0));
    redHexagonPrefab->GetTransform()->SetLocalRotation(glm::vec3(0, 90, 0));
    mapHexTile = redHexagonPrefab->AddComponent<MapHexTile>();

    comp = redHexagonPrefab->AddComponent<MeshRenderer>();
    comp->AddMaterial(MaterialsManager::GetMaterial("RedHexTile"));
    comp->SetModel(modelHexagon);

    PrefabManager::SaveAsPrefab(redHexagonPrefab, "res/prefabs/tilemap/tiles/redHexTile.prefab");
    GameObject* blueHexagonPrefab = new GameObject();
    blueHexagonPrefab->GetTransform()->Translate(glm::vec3(4, 5, 0));
    blueHexagonPrefab->GetTransform()->SetLocalRotation(glm::vec3(0, 90, 0));
    mapHexTile = blueHexagonPrefab->AddComponent<MapHexTile>();

    comp = blueHexagonPrefab->AddComponent<MeshRenderer>();
    comp->AddMaterial(MaterialsManager::GetMaterial("BlueHexTile"));
    comp->SetModel(modelHexagon);

    PrefabManager::SaveAsPrefab(blueHexagonPrefab, "res/prefabs/tilemap/tiles/blueHexTile.prefab");
    GameObject* greenHexagonPrefab = new GameObject();
    greenHexagonPrefab->GetTransform()->Translate(glm::vec3(5, 6, 0));
    greenHexagonPrefab->GetTransform()->SetLocalRotation(glm::vec3(0, 90, 0));
    mapHexTile = greenHexagonPrefab->AddComponent<MapHexTile>();

    comp = greenHexagonPrefab->AddComponent<MeshRenderer>();
    {GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		SPDLOG_ERROR("RDMError0: {}", error);
	}}
    comp->AddMaterial(MaterialsManager::GetMaterial("GreenHexTile"));
    {GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		SPDLOG_ERROR("RDMError01: {}", error);
	}}
    comp->SetModel(modelHexagon);


    //PrefabManager::SaveAsPrefab(greenHexagonPrefab, "res/prefabs/tilemap/tiles/greenHexTile.prefab");

    // TILEMAP
    //*
    GameObject* tilemapGO = new GameObject();
    HexagonalTilemap* hexagonalTilemap = tilemapGO->AddComponent<HexagonalTilemap>();
    MapGenerator* mapGenerator = tilemapGO->AddComponent<MapGenerator>();
    mapGenerator->tilemap = hexagonalTilemap;
    mapGenerator->preafabHexagonalTile = hexagonPrefab;
    mapGenerator->filledTile = blueHexagonPrefab;
    mapGenerator->pointTile = redHexagonPrefab;
    mapGenerator->additionalTile = greenHexagonPrefab;

    mapGenerator->generationRadiusMin = 7;
    mapGenerator->generationRadiusMax = 7;
    float tilemapGenerating = glfwGetTime();
    mapGenerator->Generate();
    spdlog::info("Tilemap generation: {}", glfwGetTime() - tilemapGenerating);

    PrefabManager::SaveAsPrefab(tilemapGO, "res/prefabs/tilemap/tilemap.prefab");

    //GameObject* prefabSector = new GameObject();
    //prefabSector->AddComponent<MapSector>();
    //PrefabManager::SaveAsPrefab(prefabSector, "res/prefabs/tilemap/mapSector.prefab");
    //GameObject* prefabRegion = new GameObject();
    //prefabRegion->AddComponent<MapRegion>();
    //PrefabManager::SaveAsPrefab(prefabRegion, "res/prefabs/tilemap/mapRegion.prefab");

    //ContentGenerator* contentGenerator = tilemapGO->AddComponent<ContentGenerator>();
   //SectorsGenerator sectorsGenertor;
   //sectorsGenertor.minTilesPerSector = 3;
   //sectorsGenertor.maxTilesPerSector = 3;
   //sectorsGenertor.prefabSector = prefabSector;
   //contentGenerator->mapElementGenerators.push_back(&sectorsGenertor);
   //RegionsBySectorsGenerator regionsBySectorsGenerator;
   //regionsBySectorsGenerator.regionPrefab = prefabRegion;
   //regionsBySectorsGenerator.mergeByNumberTilesPerRegion = false;
   //regionsBySectorsGenerator.minSectorsPerRegion = 3;
   //regionsBySectorsGenerator.maxSectorsPerRegion = 3;
   //regionsBySectorsGenerator.lowerHeightRange = 0;
   //regionsBySectorsGenerator.upperHeightRange = 3;
   //regionsBySectorsGenerator.heightRangeFacor = 0.25f;
   //regionsBySectorsGenerator.isDiscritizedHeight = true;
   //contentGenerator->mapElementGenerators.push_back(&regionsBySectorsGenerator);

   // RegionsGeneratorByKMeans regionsGeneratorKMeans;
    //regionsGeneratorKMeans.regionPrefab = prefabRegion;
    //regionsGeneratorKMeans.isDiscritizedHeight = true;
    //regionsGeneratorKMeans.lowerHeightRange = 0;
    //regionsGeneratorKMeans.upperHeightRange = 3;
    //regionsGeneratorKMeans.heightRangeFacor = 0.25f;
    //regionsGeneratorKMeans.regionsCount = 10;
    //contentGenerator->mapElementGenerators.push_back(&regionsGeneratorKMeans);

    //SectorGeneratorForRegionsByKMeans sectorGeneratorsKMeans;
    //sectorGeneratorsKMeans.sectorPrefab = prefabSector;
    //sectorGeneratorsKMeans.sectorsCount = 3;
    //sectorGeneratorsKMeans.isDiscritizedHeight = true;
    //sectorGeneratorsKMeans.lowerHeightRange = 0;
    //sectorGeneratorsKMeans.upperHeightRange = 2;
    //sectorGeneratorsKMeans.heightRangeFacor = 0.125f;
    //contentGenerator->mapElementGenerators.push_back(&sectorGeneratorsKMeans);

    //LakeGenerator lakeGenerator;
    //lakeGenerator.numberOfLakes = 2;
    //lakeGenerator.waterLevel = -5.f;
    //lakeGenerator.destroyWaterTile = true;
    //contentGenerator->mapElementGenerators.push_back(&lakeGenerator);


    //InstatiatingModel mountainModel = ModelsManager::LoadModel("res/models/mountain.obj");
    //GameObject* mountainPrefab = new GameObject();
    //comp = mountainPrefab->AddComponent<MeshRenderer>();
    //comp->AddMaterial(MaterialsManager::GetMaterial("MountainsUnlit"));
    //comp->SetModel(mountainModel);
    //PrefabManager::SaveAsPrefab(mountainPrefab, "res/prefabs/tilemap/mapElements/mountains.prefab");
    //MountainsGenerator mountainsGenerator;
    //mountainsGenerator.prefabMountains = mountainPrefab;
    //mountainsGenerator.mountainsHeight = 0.2f;
    //mountainsGenerator.mountainsNumber = 3;
    //contentGenerator->mapElementGenerators.push_back(&mountainsGenerator);

    //InstatiatingModel cityModel = ModelsManager::LoadModel("res/models/city.obj");
    //GameObject* cityPrefab = new GameObject();
    ////cityPrefab->GetTransform()->Scale(glm::vec3(1.0f, 0.5f, 1.0f));
    //comp = cityPrefab->AddComponent<MeshRenderer>();
    //comp->AddMaterial(MaterialsManager::GetMaterial("CityMaterial"));
    //comp->SetModel(cityModel);
    //CitiesGenerator cityGenerator;
    //cityGenerator.prefabCity = cityPrefab;
    //cityGenerator.density = 1.0f;
    //contentGenerator->mapElementGenerators.push_back(&cityGenerator);

    //PrefabManager::SaveAsPrefab(cityPrefab, "res/prefabs/tilemap/mapElements/city.prefab");
    //
    //InstatiatingModel radioStationModel = ModelsManager::LoadModel("res/models/radioStation.obj");
    //GameObject* radioStationPrefab = new GameObject();
    //radioStationPrefab->GetTransform()->Scale(glm::vec3(1.0f, 0.5f, 1.0f));
    //comp = radioStationPrefab->AddComponent<MeshRenderer>();
    //comp->AddMaterial(MaterialsManager::GetMaterial("Basic2"));
    //comp->SetModel(radioStationModel);
    //RadioStationGeneratorSectorBased radioStationGenerator;

    //PrefabManager::SaveAsPrefab(radioStationPrefab, "res/prefabs/tilemap/mapElements/radioStation.prefab");
    //radioStationGenerator.prefabRadioStation = radioStationPrefab;
    //radioStationGenerator.densityFactorPerSector = 0.1f;
    //contentGenerator->mapElementGenerators.push_back(&radioStationGenerator);


    //tilemapGenerating = glfwGetTime();
    //contentGenerator->GenerateContent(hexagonalTilemap);
    //spdlog::info("Tilemap content generation: {}", glfwGetTime() - tilemapGenerating);
    /**/
    //hexagonPrefab->SetActive(false);
    //greenHexagonPrefab->SetActive(false);
    //redHexagonPrefab->SetActive(false);
    //blueHexagonPrefab->SetActive(false);
    //mountainPrefab->SetActive(false);
    //cityPrefab->SetActive(false);
    //radioStationPrefab->SetActive(false);
    //mountainPrefab->GetTransform()->Translate(glm::vec3(2, 6, 0));
    //mountainPrefab->GetTransform()->SetLocalRotation(glm::vec3(0, 90, 0));
    //cityPrefab->GetTransform()->Translate(glm::vec3(2, 6, 0));
    //cityPrefab->GetTransform()->SetLocalRotation(glm::vec3(0, 90, 0));
    //radioStationPrefab->GetTransform()->Translate(glm::vec3(2, 6, 0));
    //radioStationPrefab->GetTransform()->SetLocalRotation(glm::vec3(0, 90, 0));

#pragma endregion

    CollisionSystem::CollisionManager::Instance()->PerformCollisions();/**/
    
    SceneManager::LoadScene("testScene");
    //SceneManager::SaveScene("res/scenes/savedScene.yaml");

    //InstatiatingModel modelHexagon = ModelsManager::LoadModel("res/models/hexagon.obj");
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
    LightingController::Instance()->SetViewerPosition(cameraPos);
    LightingController::Instance()->SetAmbientLight(glm::vec3(0.2f, 0.2f, 0.2f));
    LightingController::Instance()->SetHighlightParam(2.0f);
#pragma endregion

    // Main loop
    while (!window->IsClosed())
    {
        // Process I/O operations here
        input();

        if (window->IsClosed()) break;

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
    glfwWindowHint(GLFW_OPENGL_PROFILE,        GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
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
        glm::vec3 cp = c->GetTransform()->GetGlobalPosition();
        LightingSystem::LightingController::Instance()->SetViewerPosition(cp);
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
    LightingSystem::LightingController::Instance()->ViewerTransformChanged.Invoke();
}

void update()
{
    // Update game objects' state here
    text->SetText("Time: " + std::to_string(Time::GetDeltaTime()));
    SceneManager::UpdateCurrentScene();

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
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

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
        ImGui::Begin("Twin^2 Engine");

        ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), "Hello World!");

        if (ImGui::CollapsingHeader("Help")) {
            ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "Press Left ALT to disable GUI and start Moving Camera");
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), "Move Camera using WASD");
            ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), "Rotate Camera using Mouse");
            ImGui::Spacing();
        }

        ImGui::Separator();

#pragma region IMGUI_AUDIO_SETUP
        if (ImGui::CollapsingHeader("Audio")) {

            AudioComponent* a = Camera->GetComponent<AudioComponent>();
            bool loop = a->IsLooping();

            if (ImGui::Checkbox("Loop", &loop)) {
                if (loop) {
                    if (!a->IsLooping()) {
                        a->Loop();
                    }
                }
                else {
                    if (a->IsLooping()) {
                        a->UnLoop();
                    }
                }
            }

            float vol = a->GetVolume();

            ImGui::SliderFloat("Volume", &vol, 0.f, 1.f);

            if (a->GetVolume() != vol) {
                a->SetVolume(vol);
            }

            ImGui::Text("Position: %02.0f:%02.0f / %02.0f:%02.0f", std::floor(a->GetPlayPosition() / 60), mod(a->GetPlayPosition(), 60), std::floor(a->GetAudioLength() / 60), mod(a->GetAudioLength(), 60));
            ImGui::Text("Play Time: %02.0f:%02.0f", std::floor(a->GetPlayTime() / 60), mod(a->GetPlayTime(), 60));

            if (ImGui::Button("Play Song")) {
                Camera->GetComponent<AudioComponent>()->Play();
            }

            if (ImGui::Button("Pause Song")) {
                Camera->GetComponent<AudioComponent>()->Pause();
            }

            if (ImGui::Button("Stop Song")) {
                Camera->GetComponent<AudioComponent>()->Stop();
            }
        }
#pragma endregion

        ImGui::Separator();

#pragma region IMGUI_CAMERA_SETUP
        if (ImGui::CollapsingHeader("Main Camera")) {

            CameraComponent* c = CameraComponent::GetMainCamera();

            RenderResolution res = c->GetRenderResolution();

            if (ImGui::BeginCombo("Render Resolution", res == RenderResolution::DEFAULT ? "Default" : (res == RenderResolution::MEDIUM ? "Medium" : "High")))
            {
                if (ImGui::Selectable("Default", res == RenderResolution::DEFAULT))
                {
                    c->SetRenderResolution(RenderResolution::DEFAULT);
                }
                else if (ImGui::Selectable("Medium", res == RenderResolution::MEDIUM))
                {
                    c->SetRenderResolution(RenderResolution::MEDIUM);
                }
                else if (ImGui::Selectable("High", res == RenderResolution::HIGH))
                {
                    c->SetRenderResolution(RenderResolution::HIGH);
                }
                ImGui::EndCombo();
            }

            uint8_t acFil = RenderFilter::NONE;
            uint8_t fil = c->GetCameraFilters();

            bool g = (fil & RenderFilter::GRAYSCALE) != 0;

            ImGui::Checkbox("GrayScale", &g);
            if (g) {
                acFil |= RenderFilter::GRAYSCALE;
            }

            g = (fil & RenderFilter::NEGATIVE) != 0;

            ImGui::Checkbox("Negative", &g);
            if (g) {
                acFil |= RenderFilter::NEGATIVE;
            }

            g = (fil & RenderFilter::VIGNETTE) != 0;

            ImGui::Checkbox("Vignette", &g);
            if (g) {
                acFil |= RenderFilter::VIGNETTE;
            }

            g = (fil & RenderFilter::BLUR) != 0;

            ImGui::Checkbox("Blur", &g);
            if (g) {
                acFil |= RenderFilter::BLUR;
            }

            g = (fil & RenderFilter::DEPTH) != 0;

            ImGui::Checkbox("Depth", &g);
            if (g) {
                acFil |= RenderFilter::DEPTH;
            }

            c->SetCameraFilter(acFil);

            int s = (int)c->GetSamples();

            ImGui::InputInt("MSAA Samples", &s);

            if (s != (int)c->GetSamples()) {
                c->SetSamples((uint8_t)s);
            }

            bool per = (c->GetCameraType() == CameraType::PERSPECTIVE);

            if (ImGui::Button((per ? "Orthographic" : "Perspective"))) {
                if (per) {
                    c->SetCameraType(CameraType::ORTHOGRAPHIC);
                }
                else {
                    c->SetCameraType(CameraType::PERSPECTIVE);
                }
            }

            float n = c->GetNearPlane();
            ImGui::InputFloat("Near Plane", &n);

            if (n != c->GetNearPlane()) {
                c->SetNearPlane(n);
            }

            float f = c->GetFarPlane();
            ImGui::InputFloat("Far Plane", &f);

            if (f != c->GetFarPlane()) {
                c->SetFarPlane(f);
            }

            float gm = c->GetGamma();
            ImGui::InputFloat("Gamma", &gm);

            if (gm != c->GetGamma()) {
                c->SetGamma(gm);
            }
        }
#pragma endregion
        
        ImGui::Separator();

#pragma region IMGUI_WINDOW_SETUP
        if (ImGui::CollapsingHeader("Window Setup")) {

            // Window Settings
            if (window->IsWindowed()) {
                ImGui::Text("Current State: Windowed");

                int monitorsCount;
                GLFWmonitor** monitors = glfwGetMonitors(&monitorsCount);

                ImGui::Text("Monitors: ");
                for (int i = 0; i < monitorsCount; ++i) {
                    int x, y, mw, mh;
                    float sx, sy;

                    glfwGetMonitorPos(monitors[i], &x, &y);
                    const GLFWvidmode* vid = glfwGetVideoMode(monitors[i]);
                    const char* name = glfwGetMonitorName(monitors[i]);
                    glfwGetMonitorPhysicalSize(monitors[i], &mw, &mh);
                    glfwGetMonitorContentScale(monitors[i], &sx, &sy);

                    std::string btnText = std::to_string(i) + ". " + name + ": PS " + std::to_string(mw) + "x" + std::to_string(mh) + ", S " \
                        + std::to_string(vid->width) + "x" + std::to_string(vid->height) + \
                        ", Pos " + std::to_string(x) + "x" + std::to_string(y) + \
                        ", Scale " + std::to_string(sx) + "x" + std::to_string(sy) + \
                        ", Refresh " + std::to_string(vid->refreshRate) + " Hz";
                    if (ImGui::Button(btnText.c_str())) {
                        window->SetFullscreen(monitors[i]);
                    }
                }

                ImGui::Text("");
                static char tempBuff[256] = "Twin^2 Engine";
                ImGui::InputText("Title", tempBuff, 256);
                if (std::string(tempBuff) != window->GetTitle()) {
                    window->SetTitle(std::string(tempBuff));
                }

                if (ImGui::Button("Request Attention")) {
                    window->RequestAttention();
                }

                if (ImGui::Button("Maximize")) {
                    window->Maximize();
                }

                if (ImGui::Button("Hide")) {
                    window->Hide();
                }

                bool temp = window->IsResizable();
                if (ImGui::Button(((temp ? string("Disable") : string("Enable")) + string(" Resizability")).c_str())) {
                    window->EnableResizability(!temp);
                }

                temp = window->IsDecorated();
                if (ImGui::Button(((temp ? string("Disable") : string("Enable")) + string(" Decorations")).c_str())) {
                    window->EnableDecorations(!temp);
                }

                static float opacity = window->GetOpacity();
                ImGui::SliderFloat("Opacity", &opacity, 0.f, 1.f);
                if (opacity != window->GetOpacity()) {
                    window->SetOpacity(opacity);
                }

                static glm::ivec2 ratio = window->GetAspectRatio();
                ImGui::InputInt2("Aspect Ratio", (int*)&ratio);
                if (ImGui::Button("Apply")) {
                    window->SetAspectRatio(ratio);
                    ratio = window->GetAspectRatio();
                }
            }
            else {
                ImGui::Text("Current State: Fullscreen");
                if (ImGui::Button("Windowed")) {
                    window->SetWindowed({ 0, 30 }, { WINDOW_WIDTH, WINDOW_HEIGHT - 50 });
                }

                static int refreshRate = window->GetRefreshRate();
                ImGui::InputInt("Refresh Rate", &refreshRate);
                if (ImGui::Button("Apply")) {
                    window->SetRefreshRate(refreshRate);
                    refreshRate = window->GetRefreshRate();
                }
            }

            if (ImGui::Button("Minimize")) {
                window->Minimize();
            }

            static glm::ivec2 size = window->GetWindowSize();
            ImGui::InputInt2("Window Size", (int*)&size);
            if (ImGui::Button("Apply")) {
                window->SetWindowSize(size);
                size = window->GetWindowSize();
            }

            if (ImGui::Button(((window->IsVSyncOn() ? "Disable"s : "Enable"s) + " VSync"s).c_str())) {
                window->EnableVSync(!window->IsVSyncOn());
            }
        }
#pragma endregion

        ImGui::Separator();

        ImGui::Checkbox("IsFrustumCullingOn", &(CameraComponent::GetMainCamera()->IsFrustumCullingOn));

        ImGui::Separator();

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

#pragma region ScriptableObjects

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
        ImGui::InputText("##dstPath", dstPath, 254);

        if (ImGui::Button("Create ScriptableObject"))
        {
            SPDLOG_WARN("Button pressed");
            if (selectedSO.size() > 0)
            {
                SPDLOG_WARN("Selected size");
                string strDstPath(dstPath);
                if (strDstPath.size() > 0)
                {
                    SPDLOG_WARN("DstPath size");
                    ScriptableObjectManager::CreateScriptableObject(strDstPath, selectedSO);
                }
            }
        }
#pragma endregion

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

float fmapf(float input, float currStart, float currEnd, float expectedStart, float expectedEnd) 
{
    return expectedStart + ((expectedEnd - expectedStart) / (currEnd - currStart)) * (input - currStart);
}

double mod(double val1, double val2) {
    if (val1 < 0 && val2 <= 0) {
        return 0;
    }

    double x = val1 / val2;
    double z = std::floor(val1 / val2);
    return (x - z) * val2;
}