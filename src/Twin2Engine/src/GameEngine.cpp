#include <GameEngine.h>

using namespace Twin2Engine;
using namespace Twin2Engine::Tools;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::UI;
using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Physic;
using namespace Twin2Engine::Processes;

using namespace GameScripts;

using Twin2Engine::Core::Input;
using Twin2Engine::Core::KEY;
using Twin2Engine::Core::MOUSE_BUTTON;
using Twin2Engine::Core::CURSOR_STATE;
using Twin2Engine::Core::Time;

bool GameEngine::updateShadowLightingMap = false;

void GameEngine::Deserializers()
{
    // COMPONENTS DESELIALIZERS
    ADD_COMPONENT("Camera", CameraComponent);

    ADD_COMPONENT("Audio", AudioComponent);

    ADD_COMPONENT("Button", Button);

    ADD_COMPONENT("Image", Image);

    ADD_COMPONENT("Text", Text);

    ADD_COMPONENT("MeshRenderer", MeshRenderer);

    ADD_COMPONENT("HexagonalCollider", HexagonalColliderComponent);

    ADD_COMPONENT("SphereCollider", SphereColliderComponent);

    ADD_COMPONENT("BoxCollider", BoxColliderComponent);

    ADD_COMPONENT("CapsuleCollider", CapsuleColliderComponent);

    ADD_COMPONENT("DirectionalLight", DirectionalLightComponent);

    ADD_COMPONENT("PointLight", PointLightComponent);

    ADD_COMPONENT("SpotLight", SpotLightComponent);

    ADD_COMPONENT("MovementController", MovementController);
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

#if TRACY_PROFILER
const char* const tracy_FrameName = "Frame";
const char* const tracy_OnInputFrameName = "OnInput";
const char* const tracy_UpdateFrameName = "Update";
const char* const tracy_RenderFrameName = "Render";
const char* const tracy_EndFrameName = "EndFrame";
const char* const tracy_SceneManagerUpdateName = "SceneManagerUpdate";
const char* const tracy_TimeUpdateName = "TimeUpdate";
const char* const tracy_InputUpdateName = "InputUpdate";
const char* const tracy_WindowUpdateName = "WindowUpdate";
#endif

void GameEngine::EndFrame()
{
#if TRACY_PROFILER
    ZoneScoped;
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    FrameMarkStart(tracy_SceneManagerUpdateName);
#endif

    SceneManager::Update();

#if TRACY_PROFILER
    FrameMarkEnd(tracy_SceneManagerUpdateName);
    FrameMarkStart(tracy_TimeUpdateName);
#endif

    Time::Update();

#if TRACY_PROFILER
    FrameMarkEnd(tracy_TimeUpdateName);
    FrameMarkStart(tracy_InputUpdateName);
#endif

    Input::Update();

#if TRACY_PROFILER
    FrameMarkEnd(tracy_InputUpdateName);
    FrameMarkStart(tracy_WindowUpdateName);
#endif

    Window::GetInstance()->Update();

#if TRACY_PROFILER
    FrameMarkEnd(tracy_WindowUpdateName);
    TracyGpuCollect;
#endif
}

void GameEngine::Loop()
{
#if TRACY_PROFILER
    ZoneScoped;
    TracyGpuContext;
#endif

    // Main loop
    while (!Window::GetInstance()->IsClosed())
    {
#if TRACY_PROFILER
        FrameMarkNamed(tracy_FrameName);
        // Process I/O operations here
        FrameMarkStart(tracy_OnInputFrameName);
#endif

        OnInput();
        
#if TRACY_PROFILER
        FrameMarkEnd(tracy_OnInputFrameName);

        // Update game objects' state here
        FrameMarkStart(tracy_UpdateFrameName);
#endif

        Update();

#if TRACY_PROFILER
        FrameMarkEnd(tracy_UpdateFrameName);

        // OpenGL rendering code here
        FrameMarkStart(tracy_RenderFrameName);
#endif

        Render();

#if TRACY_PROFILER
        FrameMarkEnd(tracy_RenderFrameName);

        // End frame and swap buffers (double buffering)
        FrameMarkStart(tracy_EndFrameName);
#endif

        EndFrame();

#if TRACY_PROFILER
        FrameMarkEnd(tracy_EndFrameName);
#endif
    }
}

void GameEngine::End()
{
    // Cleanup
    EarlyEnd();

    PrefabManager::UnloadAll();
    SceneManager::UnloadAll();
    AudioManager::UnloadAll();
    CollisionManager::UnloadAll();
    Input::FreeAllWindows();

    GraphicEngine::End();
    ScriptableObjectManager::UnloadAll();
    ProcessManager::DeleteInstance();
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

	Deserializers();

	return true;
}

void GameEngine::Start()
{
#if TRACY_PROFILER
    tracy::SetThreadName("GameEngine");
#endif
    Loop();
    End();
}