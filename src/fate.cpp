#include <iostream>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "constants.h"
#include "fate.h"
#include "model.h"
#include "viewport.h"

FATE::FATE() {
    initGLFW();
    initImGui();
    viewport = std::make_unique<Viewport>(meshSelector.selectedMeshes);
}

FATE::~FATE() {
    destroyImGui();
    destroyGLFW();
}

void FATE::run() {
    auto nextFrame = std::chrono::system_clock::now() + std::chrono::duration<double, std::ratio<1, constants::FPS>>{ 0 };
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        createImGuiFrame();
        createImGuiDockspace();

        modelSelector.draw();
        if (modelSelector.modelChanged) {
            model = std::make_unique<Model>(modelSelector.modelPath);
            meshSelector.setModel(model.get());
            animationSelector.setModel(model.get());
            viewport->changeModel(model.get(), nextFrame);
            modelSelector.modelChanged = false;
        }
        meshSelector.draw();
        animationSelector.draw();
        if (animationSelector.animChanged) {
            viewport->changeAnimation(&model->animations[animationSelector.selectedAnim], nextFrame);
        }
        viewport->draw(nextFrame);
        renderImGui();

        glfwSwapBuffers(window);

        nextFrame += std::chrono::duration<double, std::ratio<1, constants::FPS>>{ 1 };
        std::this_thread::sleep_until(nextFrame);
    }
}

void FATE::initGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_WIDTH, "Model Viewer", nullptr, nullptr);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << '\n';
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << '\n';
    }

    glfwSetWindowUserPointer(window, this);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
}

void FATE::destroyGLFW() const {
    glfwTerminate();
}

void FATE::initImGui() const {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

void FATE::destroyImGui() const {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void FATE::createImGuiFrame() const {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void FATE::createImGuiDockspace() const {
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
}

void FATE::renderImGui() const {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
