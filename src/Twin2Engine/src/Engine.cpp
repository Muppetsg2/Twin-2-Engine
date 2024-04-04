#include <Engine.h>

using namespace Twin2Engine;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::UI;
using namespace Twin2Engine::GraphicEngine;

using Twin2Engine::Core::KEY;
using Twin2Engine::Core::MOUSE_BUTTON;
using Twin2Engine::Core::CURSOR_STATE;

#pragma region OpenGLCallbackFunctions

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void glfw_error_callback(int error, const char* description)
{
    spdlog::error("Glfw Error {0}: {1}\n", error, description);
}

static void GLAPIENTRY ErrorMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    spdlog::error("GL CALLBACK: {0} type = 0x{1:x}, severity = 0x{2:x}, message = {3}\n", (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
}

#pragma endregion

bool Engine::Init(const string& windowName, int32_t windowWidth, int32_t windowHeight)
{
    if (!Init_OpenGL(windowName, windowWidth, windowHeight))
    {
        spdlog::error("Failed to initialize project!");
        return false;
    }
    spdlog::info("Initialized project.");

    Init_Imgui();
    spdlog::info("Initialized ImGui.");

    SoLoud::result res = AudioManager::Init();
    if (res != 0) {
        spdlog::error(AudioManager::GetErrorString(res));
        return false;
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

    _graphicEngine = new GraphicEngine::GraphicEngine();
}

bool Engine::Init_OpenGL(const string& windowName, int32_t windowWidth, int32_t windowHeight)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
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

    _mainWindow = Window::MakeWindow(windowName, { windowWidth, windowHeight }, false);
    glfwSetFramebufferSizeCallback(_mainWindow->GetWindow(), framebuffer_size_callback);
    Input::InitForWindow(_mainWindow);

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

#pragma region MatricesUBO

    glGenBuffers(1, &UBOMatrices);

    glBindBuffer(GL_UNIFORM_BUFFER, UBOMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBOMatrices, 0, 2 * sizeof(glm::mat4));

    glBindBuffer(GL_UNIFORM_BUFFER, UBOMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(glm::mat4(1.f)));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(glm::mat4(1.f)));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

#pragma endregion

    return true;
}

bool Engine::Init_Imgui()
{
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    ImGui_ImplGlfw_InitForOpenGL(_mainWindow->GetWindow(), true);
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
    return true;
}

void Engine::Input()
{
    onInputEvent();
}

void Engine::Update()
{
    // Update game objects' state here
    onUpdateEvent();
}

void Engine::Render()
{
    // OpenGL Rendering code goes here
    for (auto& comp : RenderableComponent::_components) {
        comp->Render();
    }
    _graphicEngine->Render();
}

void Engine::Imgui_Begin()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Engine::Imgui_Render()
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

        onImguiRenderEvent();

        ImGui::Separator();

#pragma region IMGUI_WINDOW_SETUP
        if (ImGui::CollapsingHeader("Window Setup")) {

            // Window Settings
            if (_mainWindow->IsWindowed()) {
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
                        _mainWindow->SetFullscreen(monitors[i]);
                    }
                }

                ImGui::Text("");
                static char tempBuff[256] = "Twin^2 Engine";
                ImGui::InputText("Title", tempBuff, 256);
                if (std::string(tempBuff) != _mainWindow->GetTitle()) {
                    _mainWindow->SetTitle(std::string(tempBuff));
                }

                if (ImGui::Button("Request Attention")) {
                    _mainWindow->RequestAttention();
                }

                if (ImGui::Button("Maximize")) {
                    _mainWindow->Maximize();
                }

                if (ImGui::Button("Hide")) {
                    _mainWindow->Hide();
                }

                bool temp = _mainWindow->IsResizable();
                if (ImGui::Button(((temp ? "Disable"s : "Enable"s) + " Resizability"s).c_str())) {
                    _mainWindow->EnableResizability(!temp);
                }

                temp = _mainWindow->IsDecorated();
                if (ImGui::Button(((temp ? "Disable"s : "Enable"s) + " Decorations"s).c_str())) {
                    _mainWindow->EnableDecorations(!temp);
                }

                static float opacity = _mainWindow->GetOpacity();
                ImGui::SliderFloat("Opacity", &opacity, 0.f, 1.f);
                if (opacity != _mainWindow->GetOpacity()) {
                    _mainWindow->SetOpacity(opacity);
                }

                static glm::ivec2 ratio = _mainWindow->GetAspectRatio();
                ImGui::InputInt2("Aspect Ratio", (int*)&ratio);
                if (ImGui::Button("Apply")) {
                    _mainWindow->SetAspectRatio(ratio);
                    ratio = _mainWindow->GetAspectRatio();
                }
            }
            else {
                ImGui::Text("Current State: Fullscreen");
                if (ImGui::Button("Windowed")) {
                    _mainWindow->SetWindowed({ 0, 30 }, _mainWindow->GetContentSize() - glm::ivec2{ 0, 50 });
                }

                static int refreshRate = _mainWindow->GetRefreshRate();
                ImGui::InputInt("Refresh Rate", &refreshRate);
                if (ImGui::Button("Apply")) {
                    _mainWindow->SetRefreshRate(refreshRate);
                    refreshRate = _mainWindow->GetRefreshRate();
                }
            }

            if (ImGui::Button("Minimize")) {
                _mainWindow->Minimize();
            }

            static glm::ivec2 size = _mainWindow->GetWindowSize();
            ImGui::InputInt2("Window Size", (int*)&size);
            if (ImGui::Button("Apply")) {
                _mainWindow->SetWindowSize(size);
                size = _mainWindow->GetWindowSize();
            }

            if (ImGui::Button(((_mainWindow->IsVSyncOn() ? "Disable"s : "Enable"s) + " VSync"s).c_str())) {
                _mainWindow->EnableVSync(!_mainWindow->IsVSyncOn());
            }
        }
#pragma endregion

        ImGui::Separator();

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::End();
    }
}

void Engine::Imgui_End()
{
    ImGui::Render();
    _mainWindow->Use();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Engine::End_Frame()
{
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    Time::Update();
    Input::Update();
    _mainWindow->Update();
}

void Engine::Cleanup()
{
    // Cleanup
    Transform* t = _rootObject.GetTransform();
    size_t del = 0;
    for (size_t i = 0; i < t->GetChildCount() - del; ++i) {
        Transform* child = t->GetChildAt(i);
        GameObject* o = child->GetGameObject();
        DeleteGameObject(o);
        t->RemoveChild(child);
        ++del;
        delete o;
    }

    SpriteManager::UnloadAll();
    TextureManager::UnloadAll();
    AudioManager::UnloadAll();
    FontManager::UnloadAll();
    Input::FreeAllWindows();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    delete _mainWindow;
    glfwTerminate();
}

GameObject* Engine::CreateGameObject()
{
    GameObject* obj = new GameObject();
    obj->GetTransform()->SetParent(_rootObject.GetTransform());
    return obj;
}

void Engine::Start()
{
    GameLoop();
    Cleanup();
}

GLuint Engine::GetUBO() const
{
    return UBOMatrices;
}

MethodEventHandler Engine::GetOnInputEvent() const
{
    return onInputEvent;
}

MethodEventHandler Engine::GetOnUpdateEvent() const
{
    return onUpdateEvent;
}

MethodEventHandler Engine::GetOnImguiRenderEvent() const
{
    return onImguiRenderEvent;
}

Window* Engine::GetMainWindow() const
{
    return _mainWindow;
}

void Engine::GameLoop()
{
    // Main loop
    while (!_mainWindow->IsClosed())
    {
        // Process I/O operations here
        Input();

        // Update game objects' state here
        Update();

        // OpenGL rendering code here
        Render();

        // Draw ImGui
        Imgui_Begin();
        Imgui_Render(); // edit this function to add your own ImGui controls
        Imgui_End(); // this call effectively renders ImGui

        // End frame and swap buffers (double buffering)
        End_Frame();
    }
}

void Engine::DeleteGameObject(GameObject* obj)
{
    Transform* t = obj->GetTransform();
    size_t del = 0;
    for (size_t i = 0; i < t->GetChildCount() - del; ++i) {
        Transform* child = t->GetChildAt(i);
        GameObject* o = child->GetGameObject();
        DeleteGameObject(o);
        t->RemoveChild(t->GetChildAt(i));
        ++del;
        delete o;
    }
}