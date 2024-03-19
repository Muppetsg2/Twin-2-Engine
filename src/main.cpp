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

#pragma region OpenGLCallbackFunctions

static void glfw_error_callback(int error, const char* description)
{
    spdlog::error("Glfw Error {0}: {1}\n", error, description);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
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
constexpr bool fullscreen = false;

GLFWmonitor* monitor = nullptr;
GLFWwindow* window = nullptr;

// Change these to lower GL version like 4.5 if GL 4.6 can't be initialized on your machine
const     char*   glsl_version     = "#version 450";
constexpr int32_t GL_VERSION_MAJOR = 4;
constexpr int32_t GL_VERSION_MINOR = 5;

ImVec4 clear_color = ImVec4(.1f, .1f, .1f, 1.f);

GLuint UBOMatrices;

SoLoud::Soloud soloud;
SoLoud::Wav sample;
SoLoud::handle sampleHandle = 0;
bool first = true;

/*
ma_engine engine;
ma_sound sound;
*/

bool musicPlaying = false;

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

    sample.load("./res/music/FurElise.wav");

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

    // Main loop
    while (!glfwWindowShouldClose(window))
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
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    Twin2EngineCore::Input::FreeAllWindows();
    glfwDestroyWindow(window);
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

    // Creates Monitor
    if (fullscreen) {
        monitor = glfwGetPrimaryMonitor();
        if (monitor == NULL) {
            spdlog::warn("Failed to create GLFW Monitor");
        }
    }

    // Create window with graphics context
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, monitor, NULL);
    if (window == NULL)
    {
        spdlog::error("Failed to create GLFW Window!");
        return false;
    }
    spdlog::info("Successfully created GLFW Window!");

    glfwMakeContextCurrent(window);
    //glfwSwapInterval(1); // Enable VSync - fixes FPS at the refresh rate of your screen
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    Twin2EngineCore::Input::InitForWindow(window);

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

    ImGui_ImplGlfw_InitForOpenGL(window, true);
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
    if (Twin2EngineCore::Input::IsKeyPressed(Twin2EngineCore::KEY::W)) {
        spdlog::info("Delta Time: {}\n", Twin2EngineCore::Time::GetDeltaTime());
    }
}

void update()
{
    // Update game objects' state here
}

void render()
{
    // OpenGL Rendering code goes here
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
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
    {
        {
            ImGui::Begin("Twin^2 Engine");

            ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), "Hello World!");

            if (ImGui::Button("Play Song")) {
                if (!musicPlaying) {
                    //ma_sound_start(&sound);
                    if (first) {
                        sampleHandle = soloud.play(sample);
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
            ImGui::End();
        }
    }
}

void imgui_end()
{
    ImGui::Render();
    glfwMakeContextCurrent(window);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void end_frame()
{
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    Twin2EngineCore::Time::Update();
    Twin2EngineCore::Input::Update();
    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
}

float fmapf(float input, float currStart, float currEnd, float expectedStart, float expectedEnd) {
    return expectedStart + ((expectedEnd - expectedStart) / (currEnd - currStart)) * (input - currStart);
}