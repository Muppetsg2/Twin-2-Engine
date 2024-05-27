#include <GameEngine.h>
#include <tracy/Tracy.hpp>

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

    ADD_COMPONENT("Cloud", Cloud);
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

const char* const tracy_FrameName = "Frame";
const char* const tracy_OnInputFrameName = "OnInput";
const char* const tracy_UpdateFrameName = "Update";
const char* const tracy_RenderFrameName = "Render";
const char* const tracy_EndFrameName = "EndFrame";
const char* const tracy_SceneManagerUpdateName = "SceneManagerUpdate";
const char* const tracy_TimeUpdateName = "TimeUpdate";
const char* const tracy_InputUpdateName = "InputUpdate";
const char* const tracy_WindowUpdateName = "WindowUpdate";

void GameEngine::EndFrame()
{
    ZoneScoped;
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    FrameMarkStart(tracy_SceneManagerUpdateName);
    SceneManager::Update();
    FrameMarkEnd(tracy_SceneManagerUpdateName);
    FrameMarkStart(tracy_TimeUpdateName);
    Time::Update();
    FrameMarkEnd(tracy_TimeUpdateName);
    FrameMarkStart(tracy_InputUpdateName);
    Input::Update();
    FrameMarkEnd(tracy_InputUpdateName);
    FrameMarkStart(tracy_WindowUpdateName);
    Window::GetInstance()->Update();
    FrameMarkEnd(tracy_WindowUpdateName);
    TracyGpuCollect;
}

void GameEngine::Loop()
{
    ZoneScoped;
    TracyGpuContext;
    // Main loop
    while (!Window::GetInstance()->IsClosed())
    {
        FrameMarkNamed(tracy_FrameName);
        // Process I/O operations here
        FrameMarkStart(tracy_OnInputFrameName);
        OnInput();
        FrameMarkEnd(tracy_OnInputFrameName);

        // Update game objects' state here
        FrameMarkStart(tracy_UpdateFrameName);
        Update();
        FrameMarkEnd(tracy_UpdateFrameName);

        // OpenGL rendering code here
        FrameMarkStart(tracy_RenderFrameName);
        Render();
        FrameMarkEnd(tracy_RenderFrameName);

        // End frame and swap buffers (double buffering)
        FrameMarkStart(tracy_EndFrameName);
        EndFrame();
        FrameMarkEnd(tracy_EndFrameName);
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

    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    //glBlendFunc(GL_ONE, GL_SRC_ALPHA); 
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	return true;
}

void GameEngine::Start()
{
    tracy::SetThreadName("GameEngine");
    Loop();
    End();
}