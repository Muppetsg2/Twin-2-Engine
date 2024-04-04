#include <Engine.h>

using namespace Twin2Engine;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::UI;

Engine engine;

constexpr int32_t WINDOW_WIDTH = 1920;
constexpr int32_t WINDOW_HEIGHT = 1080;
const char* WINDOW_NAME = "Twin^2 Engine";

Window* _mainWindow = nullptr;

#pragma region CAMERA_CONTROLLING

GameObject* Camera;

glm::vec3 cameraPos(0.f, 0.f, 5.f);

double lastX = 0.0f;
double lastY = 0.0f;

float cameraSpeed = 40.0f;
float sensitivity = 0.1f;

bool mouseNotUsed = true;

#pragma endregion

Mesh* mesh;
Shader* shader;
Material material;
Material material2;
InstatiatingModel modelMesh;
GameObject* gameObject;
GameObject* gameObject2;
GameObject* gameObject3;

GameObject* imageObj;
GameObject* textObj;
Text* text;

static float fmapf(float input, float currStart, float currEnd, float expectedStart, float expectedEnd)
{
    return expectedStart + ((expectedEnd - expectedStart) / (currEnd - currStart)) * (input - currStart);
}

static double mod(double val1, double val2) {
    if (val1 < 0 && val2 <= 0) {
        return 0;
    }

    double x = val1 / val2;
    double z = std::floor(val1 / val2);
    return (x - z) * val2;
}

static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
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

    //printf("MPosX: %f MPosY: %f\n", xpos, ypos);

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    //yaw += xoffset;
    //pitch += yoffset;

    glm::vec3 rot = Camera->GetTransform()->GetGlobalRotation();

    // YAW = ROT Y
    // PITCH = ROT X
    // ROLL = ROT Z

    Camera->GetTransform()->SetGlobalRotation(glm::vec3(rot.x + yoffset, rot.y + xoffset, rot.z));

    rot = Camera->GetTransform()->GetGlobalRotation();

    if (rot.x > 89.0f) {
        Camera->GetTransform()->SetGlobalRotation(glm::vec3(89.f, rot.y, rot.z));
    }
    if (rot.x < -89.0f)
    {
        Camera->GetTransform()->SetGlobalRotation(glm::vec3(-89.f, rot.y, rot.z));
    }

    rot = Camera->GetTransform()->GetGlobalRotation();

    glm::vec3 front{};
    front.x = cos(glm::radians(rot.y)) * cos(glm::radians(rot.x));
    front.y = sin(glm::radians(rot.x));
    front.z = sin(glm::radians(rot.y)) * cos(glm::radians(rot.x));
    Camera->GetComponent<CameraComponent>()->SetFrontDir(glm::normalize(front));

    glBindBuffer(GL_UNIFORM_BUFFER, engine.GetUBO());
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(Camera->GetComponent<CameraComponent>()->GetViewMatrix()));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

static void input() {
    if (Input::IsKeyPressed(KEY::ESCAPE))
    {
        _mainWindow->Close();
        return;
    }

    bool camDirty = false;

    CameraComponent* c = Camera->GetComponent<CameraComponent>();

    if (!Input::IsKeyUp(KEY::W))
    {
        camDirty = true;
        Camera->GetTransform()->SetGlobalPosition(Camera->GetTransform()->GetGlobalPosition() + c->GetFrontDir() * cameraSpeed * Time::GetDeltaTime());
    }
    if (!Input::IsKeyUp(KEY::S))
    {
        camDirty = true;
        Camera->GetTransform()->SetGlobalPosition(Camera->GetTransform()->GetGlobalPosition() - c->GetFrontDir() * cameraSpeed * Time::GetDeltaTime());
    }
    if (!Input::IsKeyUp(KEY::A))
    {
        camDirty = true;
        Camera->GetTransform()->SetGlobalPosition(Camera->GetTransform()->GetGlobalPosition() - c->GetRight() * cameraSpeed * Time::GetDeltaTime());
    }
    if (!Input::IsKeyUp(KEY::D))
    {
        camDirty = true;
        Camera->GetTransform()->SetGlobalPosition(Camera->GetTransform()->GetGlobalPosition() + c->GetRight() * cameraSpeed * Time::GetDeltaTime());
    }
    /*
    if (Input::IsKeyHeldDown(KEY::Q))
    {
        cameraPos -= cameraUp * cameraSpeed * Time::GetDeltaTime();
    }
    if (Input::IsKeyHeldDown(KEY::E))
    {
        cameraPos += cameraUp * cameraSpeed * Time::GetDeltaTime();
    }
    */

    /*
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
    */

    if (camDirty)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, engine.GetUBO());
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(Camera->GetComponent<CameraComponent>()->GetViewMatrix()));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    if (Input::IsKeyPressed(KEY::LEFT_ALT))
    {
        mouseNotUsed = true;
        if (Input::GetCursorState() == CURSOR_STATE::DISABLED)
        {
            Input::ShowCursor();
            glfwSetCursorPosCallback(_mainWindow->GetWindow(), ImGui_ImplGlfw_CursorPosCallback);
        }
        else
        {
            Input::HideAndLockCursor();
            glfwSetCursorPosCallback(_mainWindow->GetWindow(), mouse_callback);
        }
    }
}

static void update() {
    text->SetText("Time: " + std::to_string(Time::GetDeltaTime()));
}

static void imgui_render() {
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
}

int main() {
    if (!engine.Init(WINDOW_NAME, WINDOW_WIDTH, WINDOW_HEIGHT)) {
        return -1;
    }
    engine.GetOnInputEvent() += input;
    engine.GetOnImguiRenderEvent() += imgui_render;
    engine.GetOnUpdateEvent() += update;

    Camera = engine.CreateGameObject();
    Camera->GetTransform()->SetGlobalPosition(cameraPos);
    Camera->GetTransform()->SetGlobalRotation(glm::vec3(0.f, -90.f, 0.f));
    CameraComponent* c = Camera->AddComponent<CameraComponent>();
    c->SetIsMain(true);
    c->SetWindowSize(glm::vec2(WINDOW_WIDTH, WINDOW_HEIGHT));
    c->SetFOV(45.f);

    AudioComponent* a = Camera->AddComponent<AudioComponent>();
    a->SetAudio("./res/music/FurElise.wav");
    a->Loop();

    modelMesh = ModelsManager::GetCube();

    material = MaterialsManager::GetMaterial("Basic");
    material2 = MaterialsManager::GetMaterial("Basic2");

    gameObject = engine.CreateGameObject();
    auto comp = gameObject->AddComponent<MeshRenderer>();
    comp->AddMaterial(material);
    comp->SetModel(modelMesh);

    gameObject2 = engine.CreateGameObject();
    gameObject2->GetTransform()->Translate(glm::vec3(2, 1, 0));
    comp = gameObject2->AddComponent<MeshRenderer>();
    comp->AddMaterial(material2);
    comp->SetModel(modelMesh);

    gameObject3 = engine.CreateGameObject();
    gameObject3->GetTransform()->Translate(glm::vec3(0, -1, 0));
    comp = gameObject3->AddComponent<MeshRenderer>();
    comp->AddMaterial(material2);
    comp->SetModel(modelMesh);

    imageObj = engine.CreateGameObject();
    Image* img = imageObj->AddComponent<Image>();
    img->SetSprite(SpriteManager::MakeSprite("stone", "res/textures/stone.jpg"));
    Image* img2 = imageObj->AddComponent<Image>();
    img2->SetSprite(SpriteManager::MakeSprite("grass", "res/textures/grass.png"));

    textObj = engine.CreateGameObject();
    textObj->GetTransform()->SetGlobalPosition(glm::vec3(400, 0, 0));
    text = textObj->AddComponent<Text>();
    text->SetColor(glm::vec4(1.f));
    text->SetText("Time: ");
    text->SetSize(48);
    text->SetFont("res/fonts/arial.ttf");

    GameObject go1{};
    GameObject go2{};
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

    engine.Start();
}