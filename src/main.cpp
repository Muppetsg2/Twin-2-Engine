// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include <glad/glad.h>  // Initialize with gladLoadGL()
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#include <spdlog/spdlog.h>

// Soloud
#include <soloud.h>
#include <soloud_wav.h>

/*
// Miniaudio
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
*/

// OpenGL Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// HID
#include <core/Input.h>

// TIME
#include <core/Time.h>

// WINDOW
#include <core/Window.h>

// MANAGERS
#include <manager/TextureManager.h>
#include <manager/SpriteManager.h>

// GAME OBJECT
#include <core/GameObject.h>
#include <ui/Image.h>

// GRAPHIC_ENGINE
#include <GraphicEnigine.h>

//LOGGER
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

#pragma region CAMERA_CONTROLLING

glm::vec3 cameraPos(-5.0f, 0.0f, -5.0f);
glm::vec3 cameraFront(1.0f, 0.0f, 1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

double lastX = 0.0f;
double lastY = 0.0f;

float yaw = 45.0f;
float pitch = 0.0f;

GLFWcursorposfun lastMouseCallback;

bool mouseUsingStarted = false;

static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (mouseUsingStarted)
    {
        lastX = xpos;
        lastY = ypos;
        mouseUsingStarted = false;
    }
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = ypos - lastY; // Odwr�cone, poniewa� wsp�rz�dne zmieniaj� si� od do�u do g�ry  
    lastX = xpos;
    lastY = ypos;

    //printf("MPosX: %f MPosY: %f\n", xpos, ypos);

    GLfloat sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;


    yaw += xoffset;
    pitch -= yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front(0.f);
    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    cameraFront = glm::normalize(front);
}

#pragma endregion

using Twin2Engine::Core::Input;
using Twin2Engine::Core::KEY;
using Twin2Engine::Core::MOUSE_BUTTON;
using Twin2Engine::Core::CURSOR_STATE;
using Twin2Engine::Core::Time;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::UI;
using namespace Twin2Engine::GraphicEngine;

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

#pragma region FunctionsDeclaration

bool init();
void init_imgui();

void input();
void update();
void render();

void imgui_begin();
void imgui_render();
void imgui_end();

void end_frame();

float fmapf(float input, float currStart, float currEnd, float expectedStart, float expectedEnd);

#pragma endregion

constexpr int32_t WINDOW_WIDTH  = 1920;
constexpr int32_t WINDOW_HEIGHT = 1080;
const char* WINDOW_NAME = "Twin^2 Engine";

Window* window = nullptr;

// Change these to lower GL version like 4.5 if GL 4.6 can't be initialized on your machine
const     char*   glsl_version     = "#version 450";
constexpr int32_t GL_VERSION_MAJOR = 4;
constexpr int32_t GL_VERSION_MINOR = 5;

ImVec4 clear_color = ImVec4(.1f, .1f, .1f, 1.f);

GLuint UBOMatrices;

SoLoud::Soloud soloud;
SoLoud::Wav smusicSmple;
SoLoud::handle sampleHandle = 0;
bool first = true;

/*
ma_engine engine;
ma_sound sound;
*/

bool musicPlaying = false;


GraphicEngine* graphicEngine;

glm::mat4 projection;
glm::mat4 view;

GameObject* imageObj;

int main(int, char**)
{
#pragma region Initialization

    if (!init())
    {
        spdlog::error("Failed to initialize project!");
        return EXIT_FAILURE;
    }
    spdlog::info("Initialized project.");

    init_imgui();
    spdlog::info("Initialized ImGui.");
    
    soloud.init();
    spdlog::info("Initialized SoLoud.");

    smusicSmple.load("./res/music/FurElise.wav");

    /*
    ma_result result;
    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        return EXIT_FAILURE;
    }
    spdlog::info("Initialized MiniAudio.");

    result = ma_sound_init_from_file(&engine, "./res/music/FurElise.wav", 0, NULL, NULL, &sound);
    if (result != MA_SUCCESS) {
        return result;
    }
    */

#pragma endregion

    // Initialize stdout color sink
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::debug);

    // Create a logger with the stdout color sink
    auto logger = std::make_shared<spdlog::logger>("logger", console_sink);
    spdlog::register_logger(logger);

    // Set global log level to debug
    spdlog::set_level(spdlog::level::debug);

#pragma region MatricesUBO

    glGenBuffers(1, &UBOMatrices);

    glBindBuffer(GL_UNIFORM_BUFFER, UBOMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBOMatrices, 0, 2 * sizeof(glm::mat4));

    glBindBuffer(GL_UNIFORM_BUFFER, UBOMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(glm::perspective(glm::radians(45.f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 1000.0f)));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(glm::mat4(1.f)));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

#pragma endregion

    graphicEngine = new GraphicEngine();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    Shader* sh = ShaderManager::CreateShaderProgram("res/CompiledShaders/origin/UI.shdr", "shaders/ui.vert", "shaders/ui.frag");
    Texture2D* tex = TextureManager::LoadTexture2D("res/textures/stone.jpg");
    Sprite* s = SpriteManager::MakeSprite(tex, "stone1", 0, 0, tex->GetWidth(), tex->GetHeight());

    GameObject* ob = new GameObject();
    Image* img = ob->AddComponent<Image>();
    img->SetSprite(s);

    glEnable(GL_DEPTH_TEST);

    // Main loop
    while (!window->IsClosed())
    {
        // Process I/O operations here
        input();

        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update game objects' state here
        update();

        // OpenGL rendering code here
        render();

        // Draw ImGui
        imgui_begin();
        imgui_render(); // edit this function to add your own ImGui controls
        imgui_end(); // this call effectively renders ImGui

        // End frame and swap buffers (double buffering)
        end_frame();
    }

    // Cleanup
    delete imageObj;
    SpriteManager::UnloadAll();
    TextureManager::UnloadAll();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    delete window;
    glfwTerminate();

    return 0;
}

bool init()
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
    glfwWindowHint(GLFW_OPENGL_PROFILE,        GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    window = new Window(WINDOW_NAME, { WINDOW_WIDTH, WINDOW_HEIGHT });
    glfwSetFramebufferSizeCallback(window->GetWindow(), framebuffer_size_callback);

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

    // Depth Test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Face Culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    return true;
}

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

void input()
{
    float cameraSpeed = 1.0f; // dopasuj do swoich potrzeb  

    if (Input::IsKeyHeldDown(KEY::W))
    {
        cameraPos += cameraSpeed * cameraFront * Time::GetDeltaTime();
    }
    if (Input::IsKeyHeldDown(KEY::S))
    {
        cameraPos -= cameraSpeed * cameraFront * Time::GetDeltaTime();
    }
    if (Input::IsKeyHeldDown(KEY::A))
    {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * Time::GetDeltaTime();
    }
    if (Input::IsKeyHeldDown(KEY::D))
    {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * Time::GetDeltaTime();
    }
    if (Input::IsKeyHeldDown(KEY::Q))
    {
        cameraPos -= cameraUp * cameraSpeed * Time::GetDeltaTime();
    }
    if (Input::IsKeyHeldDown(KEY::E))
    {
        cameraPos += cameraUp * cameraSpeed * Time::GetDeltaTime();
    }
    static bool cursorToggle = false;

    if (Input::IsMouseButtonPressed(MOUSE_BUTTON::MIDDLE))
    {
        if (!cursorToggle)
        {
            lastMouseCallback = glfwSetCursorPosCallback(window->GetWindow(), mouse_callback);
            Input::HideAndLockCursor();
            cursorToggle = !cursorToggle;
            mouseUsingStarted = true;
        }
    }
    else if (Input::IsMouseButtonReleased(MOUSE_BUTTON::MIDDLE))
    {
        if (cursorToggle)
        {
            glfwSetCursorPosCallback(window->GetWindow(), lastMouseCallback);
            Input::ShowCursor();
            cursorToggle = !cursorToggle;
        }
    }
}

void update()
{
    // Update game objects' state here
}

void render()
{
    // OpenGL Rendering code goes here
    glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    graphicEngine->Render(view, projection);
    for (auto& comp : RenderableComponent::renderableComponents) {
        comp->Render();
    }
}

void imgui_begin()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void imgui_render()
{
    if (Input::GetCursorState() == NORMAL)
    {
        ImGui::Begin("Twin^2 Engine");

        ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), "Hello World!");

        if (ImGui::Button("Play Song")) {
            if (!musicPlaying) {
                //ma_sound_start(&sound);
                if (first) {
                    sampleHandle = soloud.play(smusicSmple);
                    first = false;
                }
                else {
                    soloud.setPause(sampleHandle, false);
                }
                musicPlaying = true;
            }
        }

        if (ImGui::Button("Stop Song")) {
            if (musicPlaying) {
                //ma_sound_stop(&sound);
                soloud.setPause(sampleHandle, true);
                musicPlaying = false;
            }
        }

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        
#pragma region IMGUI_WINDOW_SETUP
        ImGui::Separator();
        ImGui::Text("Window Setup");
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
            if (ImGui::Button(((temp ? "Disable"s : "Enable"s) + " Resizability"s).c_str())) {
                window->EnableResizability(!temp);
            }

            temp = window->IsDecorated();
            if (ImGui::Button(((temp ? "Disable"s : "Enable"s) + " Decorations"s).c_str())) {
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

float fmapf(float input, float currStart, float currEnd, float expectedStart, float expectedEnd) {
    return expectedStart + ((expectedEnd - expectedStart) / (currEnd - currStart)) * (input - currStart);
}