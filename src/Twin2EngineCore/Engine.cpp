#include <inc/Engine.h>

bool Twin2EngineCore::Twin2Engine::Init()
{
	return false;
}

void Twin2EngineCore::Twin2Engine::Init_Imgui()
{
}

void Twin2EngineCore::Twin2Engine::Input()
{
}

void Twin2EngineCore::Twin2Engine::Update()
{
}

void Twin2EngineCore::Twin2Engine::Render()
{
}

void Twin2EngineCore::Twin2Engine::Imgui_Begin()
{
}

void Twin2EngineCore::Twin2Engine::Imgui_Render()
{
}

void Twin2EngineCore::Twin2Engine::Imgui_End()
{
}

void Twin2EngineCore::Twin2Engine::End_Frame()
{
}

void Twin2EngineCore::Twin2Engine::GameLoop()
{
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

    GLfloat deltaTime = 0.0f; // Czas pomiêdzy obecn¹ i poprzedni¹ klatk¹  
    GLfloat lastFrame = 0.0f; // Czas ostatniej ramki

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
        // Update game time value
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process I/O operations here
        Input();

        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    Input::FreeAllWindows();
    glfwDestroyWindow(window);
    glfwTerminate();
}