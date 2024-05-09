#include <GameEngine.h>

using namespace Twin2Engine;
using namespace Twin2Engine::Tools;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::UI;
using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Physic;
using namespace Twin2Engine::Processes;

using Twin2Engine::Core::Input;
using Twin2Engine::Core::KEY;
using Twin2Engine::Core::MOUSE_BUTTON;
using Twin2Engine::Core::CURSOR_STATE;
using Twin2Engine::Core::Time;

bool GameEngine::updateShadowLightingMap = false;

void GameEngine::Deserializers()
{
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
            light->SetAtenuation(node["constant"].as<float>(), node["linear"].as<float>(), node["quadratic"].as<float>());
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
            light->SetAtenuation(node["constant"].as<float>(), node["linear"].as<float>(), node["quadratic"].as<float>());
        }
    );
}

void GameEngine::Update()
{
    EarlyUpdate();

    //Update Shadow & Lighting Map
    if (updateShadowLightingMap)
    {
        LightingController::Instance()->UpdateOnTransformChange();
        updateShadowLightingMap = false;
    }

    CollisionManager::Instance()->PerformCollisions();
    SceneManager::UpdateCurrentScene();
    Twin2Engine::Processes::ProcessManager::Instance()->UpdateSynchronizedProcess();
    LateUpdate();
}

void GameEngine::Render()
{
    EarlyRender();
    SceneManager::RenderCurrentScene();
    CameraComponent::GetMainCamera()->Render();
    LateRender();
}

void GameEngine::EndFrame()
{
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    Time::Update();
    Input::Update();
    Window::GetInstance()->Update();
}

void GameEngine::Loop()
{
    // Main loop
    while (!Window::GetInstance()->IsClosed())
    {
        // Process I/O operations here
        OnInput();

        // Update game objects' state here
        Update();

        // OpenGL rendering code here
        Render();

        // End frame and swap buffers (double buffering)
        EndFrame();
    }
}

void GameEngine::End()
{
    // Cleanup
    EarlyEnd();

    SceneManager::UnloadAll();
    AudioManager::UnloadAll();
    CollisionManager::UnloadAll();
    Input::FreeAllWindows();

    GraphicEngine::End();
}

MethodEventHandler GameEngine::OnInput;
MethodEventHandler GameEngine::EarlyUpdate;
MethodEventHandler GameEngine::LateUpdate;
MethodEventHandler GameEngine::EarlyRender;
MethodEventHandler GameEngine::LateRender;
MethodEventHandler GameEngine::EarlyEnd;

bool GameEngine::Init(const string& window_name, int32_t window_width, int32_t window_height, bool fullscreen, int32_t gl_version_major, int32_t gl_version_minor)
{
    GraphicEngine::Init(window_name, window_width, window_height, fullscreen, gl_version_major, gl_version_minor);

    if (Window::GetInstance() == nullptr) return false;
    Input::InitForWindow(Window::GetInstance());

    ScriptableObject::Init();

    SoLoud::result res = AudioManager::Init();
    if (res != 0) {
        spdlog::error(AudioManager::GetErrorString(res));
        return EXIT_FAILURE;
    }
    spdlog::info("Initialized SoLoud.");

    // Initialize stdout color sink
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::debug);

    // Create a logger with the stdout color sink
    auto logger = std::make_shared<spdlog::logger>("logger", console_sink);
    spdlog::register_logger(logger);

    // Set global log level to debug
    spdlog::set_level(spdlog::level::debug);

	Deserializers();

	return true;
}

void GameEngine::Start()
{
    Loop();
    End();
}