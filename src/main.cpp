// Soloud
#include <soloud.h>
#include <soloud_wav.h>

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
#include <ui/Image.h>
#include <ui/Text.h>

// AUDIO
#include <core/AudioComponent.h>

// GRAPHIC_ENGINE
#include <GraphicEnigineManager.h>

//LOGGER
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
#include "graphic/InstatiatingMesh.h"

// TILEMAP
#include <Tilemap/HexagonalTilemap.h>
#include <Tilemap/HexagonalTile.h>

using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::UI;
using namespace Twin2Engine::GraphicEngine;

using Twin2Engine::Core::Input;
using Twin2Engine::Core::KEY;
using Twin2Engine::Core::MOUSE_BUTTON;
using Twin2Engine::Core::CURSOR_STATE;
using Twin2Engine::Core::Time;

using Tilemap::HexagonalTile;
using Tilemap::HexagonalTilemap;

#pragma region CAMERA_CONTROLLING

GameObject Camera;

glm::vec3 cameraPos(0.f, 0.f, 5.f);

double lastX = 0.0f;
double lastY = 0.0f;

float cameraSpeed = 40.0f;
float sensitivity = 0.1f;

bool mouseNotUsed = true;

#pragma endregion

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
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void update();
void render();

void imgui_begin();
void imgui_render();
void imgui_end();

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

HexagonalTilemap hexagonalTilemap(glm::ivec2(-5, -5), glm::ivec2(5, 5), 1.f, true);

GameObject* imageObj;
GameObject* imageObj2;
GameObject* imageObj3;
Image* image;
float colorSpan = 1.f;
GameObject* textObj;
Text* text;

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

    //SoLoud::result res = soloud.init();
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

    Camera.GetTransform()->SetGlobalPosition(cameraPos);
    Camera.GetTransform()->SetGlobalRotation(glm::vec3(0.f, -90.f, 0.f));

    CameraComponent* c = Camera.AddComponent<CameraComponent>();
    c->SetFOV(45.f);

    AudioComponent* a = Camera.AddComponent<AudioComponent>();
    a->SetAudio("./res/music/FurElise.wav");
    a->Loop();

    GraphicEngineManager::Init();

    modelMesh = ModelsManager::GetCube();

    material = MaterialsManager::GetMaterial("Basic2");
    //material2 = MaterialsManager::GetMaterial("Basic2");
    //material = MaterialsManager::GetMaterial("textured");
    material2 = MaterialsManager::GetMaterial("textured");
    wallMat = MaterialsManager::GetMaterial("wallMat");
    roofMat = MaterialsManager::GetMaterial("roofMat");

    gameObject = new GameObject();
    auto comp = gameObject->AddComponent<MeshRenderer>();
    gameObject->GetTransform()->Translate(glm::vec3(2, 5, 0));
    comp->AddMaterial(material);
    comp->SetModel(modelMesh);

    gameObject2 = new GameObject();
    gameObject2->GetTransform()->Translate(glm::vec3(2, 3, 0));
    comp = gameObject2->AddComponent<MeshRenderer>();
    comp->AddMaterial(material2);
    comp->SetModel(modelMesh);

    InstatiatingModel modelCastle = ModelsManager::GetModel("res/models/castle.obj");

    gameObject3 = new GameObject();
    gameObject3->GetTransform()->Translate(glm::vec3(0, 3, 0));
    gameObject3->GetTransform()->SetLocalRotation(glm::vec3(0, 0, 0));
    comp = gameObject3->AddComponent<MeshRenderer>();
    comp->AddMaterial(wallMat);
    comp->AddMaterial(roofMat);
    comp->SetModel(modelCastle);

    InstatiatingModel modelAK = ModelsManager::GetModel("res/models/AK47.obj");

    gameObject4 = new GameObject();
    gameObject4->GetTransform()->Translate(glm::vec3(0, 0, 10));
    gameObject4->GetTransform()->Rotate(glm::vec3(90, 0, 0));
    comp = gameObject4->AddComponent<MeshRenderer>();
    comp->AddMaterial(MaterialsManager::GetMaterial("deska"));
    comp->AddMaterial(MaterialsManager::GetMaterial("metal"));
    comp->SetModel(modelAK);


    InstatiatingModel modelHexagon = ModelsManager::GetModel("res/models/hexagon.obj");

    

    GameObject* hexagonPrefab = new GameObject();
    hexagonPrefab->GetTransform()->Translate(glm::vec3(2, 3, 0));
    hexagonPrefab->GetTransform()->SetLocalRotation(glm::vec3(0, 90, 0));

    //spdlog::info("hexagon rotation: [{}, {}, {}]", hexagonPrefab->GetTransform()->GetLocalRotation().x, hexagonPrefab->GetTransform()->GetLocalRotation().y, hexagonPrefab->GetTransform()->GetLocalRotation().z);

    comp = hexagonPrefab->AddComponent<MeshRenderer>();
    comp->AddMaterial(MaterialsManager::GetMaterial("hexagonMat"));
    comp->SetModel(modelHexagon);

    hexagonalTilemap.Fill(glm::ivec2(0, 0), hexagonPrefab);
    hexagonalTilemap.GetTile(glm::ivec2(-5, -5))->GetGameObject()->GetTransform()->Translate(glm::vec3(0.0f, 1.0f, 0.0f));
    hexagonalTilemap.GetTile(glm::ivec2(5, -5))->GetGameObject()->GetTransform()->Translate(glm::vec3(0.0f, 1.0f, 0.0f));
    hexagonalTilemap.GetTile(glm::ivec2(-5, 5))->GetGameObject()->GetTransform()->Translate(glm::vec3(0.0f, 1.0f, 0.0f));
    hexagonalTilemap.GetTile(glm::ivec2(5, 5))->GetGameObject()->GetTransform()->Translate(glm::vec3(0.0f, 1.0f, 0.0f));
    
    imageObj = new GameObject();
    imageObj->GetTransform()->SetGlobalPosition(glm::vec3(-900, -500, 0));
    Image* img = imageObj->AddComponent<Image>();
    img->SetSprite(SpriteManager::MakeSprite("stone", "res/textures/stone.jpg"));
    Image* img2 = imageObj->AddComponent<Image>();
    img2->SetSprite(SpriteManager::MakeSprite("grass", "res/textures/grass.png"));

    imageObj2 = new GameObject();
    imageObj2->GetTransform()->SetGlobalPosition(glm::vec3(900, -500, 0));
    img = imageObj2->AddComponent<Image>();
    img->SetSprite("grass");

    imageObj3 = new GameObject();
    imageObj3->GetTransform()->SetGlobalPosition(glm::vec3(0, 500, 0));
    image = imageObj3->AddComponent<Image>();
    image->SetSprite(SpriteManager::MakeSprite("white_box", "res/textures/white.png"));

    textObj = new GameObject();
    textObj->GetTransform()->SetGlobalPosition(glm::vec3(400, 0, 0));
    text = textObj->AddComponent<Text>();
    text->SetColor(glm::vec4(1.f));
    text->SetText("Text");
    text->SetSize(48);
    text->SetFont("res/fonts/arial.ttf");

    GameObject go1;
    GameObject go2;
    go1.GetTransform()->SetLocalPosition(glm::vec3(1.0f, 0.0f, 0.0f));
    go2.GetTransform()->SetLocalPosition(glm::vec3(1.5f, 0.0f, 0.0f));
    Twin2Engine::Core::BoxColliderComponent* bc1 = go1.AddComponent<Twin2Engine::Core::BoxColliderComponent>();
    Twin2Engine::Core::BoxColliderComponent* bc2 = go2.AddComponent<Twin2Engine::Core::BoxColliderComponent>();
    bc1->colliderId = 1;
    bc2->colliderId = 2;
    bc1->Invoke();
    bc2->Invoke();
    bc1->Update();
    bc2->Update();

    CollisionSystem::CollisionManager::Instance()->PerformCollisions();

    // Main loop
    while (!window->IsClosed())
    {
        // Process I/O operations here
        input();

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
    delete gameObject;
    delete gameObject2;
    delete gameObject3;
    delete gameObject4;
    delete imageObj;
    delete imageObj2;
    delete imageObj3;
    delete textObj;
    SpriteManager::UnloadAll();
    TextureManager::UnloadAll();
    AudioManager::UnloadAll();
    FontManager::UnloadAll();
    Input::FreeAllWindows();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    delete Window::GetInstance();
    glfwTerminate();

    GraphicEngineManager::End();

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

    window = Window::MakeWindow(WINDOW_NAME, { WINDOW_WIDTH, WINDOW_HEIGHT }, false);
    glfwSetFramebufferSizeCallback(window->GetWindow(), framebuffer_size_callback);
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
    if (Input::IsKeyPressed(KEY::ESCAPE)) 
    {
        window->Close();
        return;
    }

    CameraComponent* c = Camera.GetComponent<CameraComponent>();

    if (Input::IsKeyDown(KEY::W))
    {
        Camera.GetTransform()->SetGlobalPosition(Camera.GetTransform()->GetGlobalPosition() + c->GetFrontDir() * cameraSpeed * Time::GetDeltaTime());
    }
    if (Input::IsKeyDown(KEY::S))
    {
        Camera.GetTransform()->SetGlobalPosition(Camera.GetTransform()->GetGlobalPosition() - c->GetFrontDir() * cameraSpeed * Time::GetDeltaTime());
    }
    if (Input::IsKeyDown(KEY::A))
    {
        Camera.GetTransform()->SetGlobalPosition(Camera.GetTransform()->GetGlobalPosition() - c->GetRight() * cameraSpeed * Time::GetDeltaTime());
    }
    if (Input::IsKeyDown(KEY::D))
    {
        Camera.GetTransform()->SetGlobalPosition(Camera.GetTransform()->GetGlobalPosition() + c->GetRight() * cameraSpeed * Time::GetDeltaTime());
    }

    if (Input::IsKeyPressed(KEY::LEFT_ALT)) 
    {
        mouseNotUsed = true;
        if (Input::GetCursorState() == CURSOR_STATE::DISABLED) 
        {
            Input::ShowCursor();
            glfwSetCursorPosCallback(window->GetWindow(), ImGui_ImplGlfw_CursorPosCallback);
        }
        else
        {
            Input::HideAndLockCursor();
            glfwSetCursorPosCallback(window->GetWindow(), mouse_callback);
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // Na potrzeby zadania
    /*
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

    glm::vec3 rot = Camera.GetTransform()->GetGlobalRotation();

    // YAW = ROT Y
    // PITCH = ROT X
    // ROLL = ROT Z

    Camera.GetTransform()->SetGlobalRotation(glm::vec3(rot.x + yoffset, rot.y + xoffset, rot.z));

    rot = Camera.GetTransform()->GetGlobalRotation();

    if (rot.x > 89.0f) {
        Camera.GetTransform()->SetGlobalRotation(glm::vec3(89.f, rot.y, rot.z));
    }
    if (rot.x < -89.0f)
    {
        Camera.GetTransform()->SetGlobalRotation(glm::vec3(-89.f, rot.y, rot.z));
    }
    */
}

float yRot = 10.f;

void update()
{
    // Update game objects' state here
    text->SetText("Time: " + std::to_string(Time::GetDeltaTime()));

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

    Camera.GetTransform()->SetGlobalRotation(Camera.GetTransform()->GetGlobalRotation() + glm::vec3(0.f, yRot * Time::GetDeltaTime(), 0.f));

    Camera.GetTransform()->Update();
}

void render()
{
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // OpenGL Rendering code goes here
    for (auto& comp : RenderableComponent::_components) {
        comp->Render();
    }
    /*
    GraphicEngineManager::Render();
    */

    CameraComponent::GetMainCamera()->Render();
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

            AudioComponent* a = Camera.GetComponent<AudioComponent>();
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
                Camera.GetComponent<AudioComponent>()->Play();
            }

            if (ImGui::Button("Pause Song")) {
                Camera.GetComponent<AudioComponent>()->Pause();
            }

            if (ImGui::Button("Stop Song")) {
                Camera.GetComponent<AudioComponent>()->Stop();
            }
        }
#pragma endregion

        ImGui::Separator();

#pragma region IMGUI_CAMERA_SETUP
        if (ImGui::CollapsingHeader("Main Camera")) {

            CameraComponent* c = CameraComponent::GetMainCamera();

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

            c->SetCameraFilter(acFil);

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