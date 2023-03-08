#include "Window.h"
#include "core.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
#include "../imgui/imgui_internal.h"

static bool isSelectAnim = false;
static bool isSelectSkel = false;
static bool isSelectAttachedSkin = false;
static bool isSelectOriginalSkin = false;
static bool isDrawSkel = false;
static bool isDrawAttachedSkin = false;
static bool isDrawOriginalSkin = false;
static bool isPlayAnim = false;
static bool isPausedJustNow = false;
float prevDeltaT;

void error_callback(int error, const char* description) {
    // Print error.
    std::cerr << description << std::endl;
}

void setup_callbacks(GLFWwindow* window) {
    // Set the error callback.
    glfwSetErrorCallback(error_callback);
    // Set the window resize callback.
    glfwSetWindowSizeCallback(window, Window::resizeCallback);

    // Set the key callback.
    glfwSetKeyCallback(window, Window::keyCallback);

    // Set the mouse and cursor callbacks
    //glfwSetMouseButtonCallback(window, Window::mouse_callback);
    //glfwSetCursorPosCallback(window, Window::cursor_callback);
}

void setup_opengl_settings() {
    // Enable depth buffering.
    glEnable(GL_DEPTH_TEST);
    // Related to shaders and z value comparisons for the depth buffer.
    glDepthFunc(GL_LEQUAL);
    // Set polygon drawing mode to fill front and back of each polygon.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // Set clear color to black.
    glClearColor(0.0, 0.0, 0.0, 0.0);
}

void print_versions() {
    // Get info of GPU and supported OpenGL version.
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version supported: " << glGetString(GL_VERSION)
        << std::endl;

    // If the shading language symbol is defined.
#ifdef GL_SHADING_LANGUAGE_VERSION
    std::cout << "Supported GLSL version is: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
#endif
}

void setImGuiStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = ImColor(12, 0, 49, 0.6 * 255);
    style.Colors[ImGuiCol_CheckMark] = ImColor(255, 114, 136, 0.8 * 255);
    style.Colors[ImGuiCol_Button] = ImColor(255, 103, 87, 0.6 * 255);
    style.Colors[ImGuiCol_ButtonActive] = ImColor(249, 255, 36, 0.8 * 255);
    style.Colors[ImGuiCol_ButtonHovered] = ImColor(87, 182, 255, 0.6 * 255);
    style.WindowMinSize = ImVec2(500, 1000);
    style.WindowTitleAlign = ImVec2(0.5, 0.5);
    style.Colors[ImGuiCol_TitleBg] = ImColor(117, 169, 255, 0.8 * 255);
    style.Colors[ImGuiCol_TitleBgActive] = ImColor(133, 101, 255, 0.6 * 255);
    //style.Colors[ImGuiCol_TitleBgCollapsed] = ImColor(180, 200, 255, 0.5 * 255);
}

void makeSliderBox(Joint* root) {
    ImGui::Text(root->JointName);

    float minX = root->JointDOF[0]->DOFmin;
    float maxX = root->JointDOF[0]->DOFmax;
    ImGui::SliderFloat(("DOF_X (" + std::string(root->JointName) + ")").c_str(), &(root->JointDOF[0]->DOFvalue), minX, maxX);

    float minY = root->JointDOF[1]->DOFmin;
    float maxY = root->JointDOF[1]->DOFmax;
    ImGui::SliderFloat(("DOF_Y (" + std::string(root->JointName) + ")").c_str(), &(root->JointDOF[1]->DOFvalue), minY, maxY);

    float minZ = root->JointDOF[2]->DOFmin;
    float maxZ = root->JointDOF[2]->DOFmax;
    ImGui::SliderFloat(("DOF_Z (" + std::string(root->JointName) + ")").c_str(), &(root->JointDOF[2]->DOFvalue), minZ, maxZ);

    for (int i = 0; i < root->children.size(); i++) {
        makeSliderBox(root->children[i]);
    }

}

int main(void) {
    // Create the GLFW window.
    GLFWwindow* window = Window::createWindow(1600, 1200);
    if (!window) exit(EXIT_FAILURE);

    // Print OpenGL and GLSL versions.
    print_versions();
    // Setup callbacks.
    setup_callbacks(window);
    // Setup OpenGL settings.
    setup_opengl_settings();

    // Initialize the shader program; exit if initialization fails.
    if (!Window::initializeProgram()) exit(EXIT_FAILURE);
    // Initialize objects/pointers for rendering; exit if initialization fails.
    if (!Window::initializeObjects()) exit(EXIT_FAILURE);

    // Setup Dear ImGui context
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    // Load Icons and Fonts
    //io.Fonts->AddFontFromFileTTF("assets/HFSprouts-2.ttf", 35.0f);
    io.Fonts->AddFontFromFileTTF("assets/Ancoa-Demo-Bold-2.otf", 45.0f);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    // Loop while GLFW window should stay open.
    while (!glfwWindowShouldClose(window)) {
        // Gets events, including input such as keyboard and mouse or window resizing.
        glfwPollEvents();
        // Clear the color and depth buffers.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        setImGuiStyle();


        // Main render display callback. Rendering of our objects.
        Window::displayCallback(window, isDrawOriginalSkin, isDrawSkel, isDrawAttachedSkin, isPlayAnim);


        // Design ImGui
        {
            ImGui::Begin("Property Panel");

            // Checkbox + Drop-down menu for choosing model (skel & skin)
            ImGui::Text("Loading Settings");
            ImGui::Checkbox("Animation", &isSelectAnim);
            if (isSelectAnim) {
                isDrawSkel = false;
                isDrawAttachedSkin = false;
                isDrawOriginalSkin = false;
                isPlayAnim = true;
                // load different anim
                static int selectedAnim = 0;
                std::vector<const char*> anims = { "Walking Wasp", "More..."};
                ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5);
                ImGui::Combo("Loading Animation", &selectedAnim, anims.data(), anims.size());
                ImGui::PopItemWidth();
                if (anims[selectedAnim] == "Walking Wasp") {
                    Window::setAnim(window, "walkingwasp");
                }
                else if (anims[selectedAnim] == "More...") {
                    ImGui::Text("<More characters are coming!!!>");
                }

                // settings for play control
                ImGui::Text("\nPlayback Settings");
                // pause
                bool isPausePushed = ImGui::Button("Pause", ImVec2(100, 60));
                if (isPausePushed && !isPausedJustNow) { // pause button
                    prevDeltaT = Window::currPlayer->deltaT;
                    Window::currPlayer->deltaT = 0;
                    isPausedJustNow = !isPausedJustNow;
                }
                else if (isPausePushed && isPausedJustNow) {
                    Window::currPlayer->deltaT = prevDeltaT;
                    isPausedJustNow = !isPausedJustNow;
                }
                // speed
                ImGui::SliderFloat("Speed", &(Window::currPlayer->playSpeed), 0.0f, 5.0f);
                // progress bar
                ImGui::SliderFloat("Time", &(Window::currPlayer->curTime), 0.0f, 100.0f);
                // play mode (after end of clip...): To infinity! loop, stop, walk backwards
                ImGui::Text("Play Mode");
                static int selectedPlayMode = NULL;
                std::vector<const char*> playModes = { "To infinity!", "Loop from start", "Stop at end", "Walk back and forth" };
                ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.55);
                ImGui::Combo("after end...", &selectedPlayMode, playModes.data(), playModes.size());
                ImGui::PopItemWidth();
                if (playModes[selectedPlayMode] == "To infinity!") {
                    Window::currPlayer->playMode = "To infinity!";
                }
                if (playModes[selectedPlayMode] == "Loop from start") {
                    Window::currPlayer->playMode = "Loop from start";
                }
                if (playModes[selectedPlayMode] == "Stop at end") {
                    Window::currPlayer->playMode = "Stop at end";
                }
                if (playModes[selectedPlayMode] == "Walk back and forth") {
                    Window::currPlayer->playMode = "Walk back and forth";
                }

                // Slider box for camera
                ImGui::Text("\nCamera Settings");
                ImGui::SliderFloat("Distance", &(Window::Cam->Distance), 1.0f, 100.0f);
                ImGui::SliderFloat("Azimuth", &(Window::Cam->Azimuth), 0.0f, 360.0f);
                ImGui::SliderFloat("Incline", &(Window::Cam->Incline), -90.0f, 90.0f);
            }
            else
            {
                isPlayAnim = false;

                ImGui::Checkbox("Skeleton", &isSelectSkel);
                if (isSelectSkel) {
                    isDrawSkel = true;
                    isDrawAttachedSkin = false;
                    isDrawOriginalSkin = false;

                    static int selectedSkel = NULL;
                    std::vector<const char*> skels = { "Tiny Man", "Static Wasp", "Dragon" };
                    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5);
                    ImGui::Combo("Loading Skeleton", &selectedSkel, skels.data(), skels.size());
                    ImGui::PopItemWidth();
                    if (skels[selectedSkel] == "Tiny Man") {
                        Window::setSkel(window, "test");
                        ImGui::Text("<No Skin File Available>");
                    }
                    else if (skels[selectedSkel] == "Static Wasp") {
                        Window::setSkel(window, "wasp1");
                        // Attach Skin
                        ImGui::Checkbox("Attach Skin", &isSelectAttachedSkin);
                        if (isSelectAttachedSkin) {
                            isDrawSkel = false;
                            isDrawAttachedSkin = true;
                            isDrawOriginalSkin = false;
                            Window::setSkin(window, "wasp1");
                        }
                    }
                    else if (skels[selectedSkel] == "Dragon") {
                        Window::setSkel(window, "dragon");
                        ImGui::Text("<No Skin File Available>");
                    }
                    // Slider box for camera
                    ImGui::Text("\nCamera Settings");
                    ImGui::SliderFloat("Distance", &(Window::Cam->Distance), 1.0f, 100.0f);
                    ImGui::SliderFloat("Azimuth", &(Window::Cam->Azimuth), 0.0f, 360.0f);
                    ImGui::SliderFloat("Incline", &(Window::Cam->Incline), -90.0f, 90.0f);
                    // Slider box for DOF
                    ImGui::Text("\nDOF Settings");
                    makeSliderBox(Window::currSkel->root);
                }
                else {
                    ImGui::Checkbox("Original Skin In Binding Space", &isSelectOriginalSkin);
                    if (isSelectOriginalSkin) {
                        isDrawSkel = false;
                        isDrawAttachedSkin = false;
                        isDrawOriginalSkin = true;

                        static int selectedSkin = NULL;
                        std::vector<const char*> skins = { "Static Wasp", "More..." };
                        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5);
                        ImGui::Combo("Loading Undeformed Skin", &selectedSkin, skins.data(), skins.size());
                        ImGui::PopItemWidth();
                        if (skins[selectedSkin] == "Static Wasp") {
                            Window::setSkin(window, "wasp1");
                        }
                        else if (skins[selectedSkin] == "More...") {
                            ImGui::Text("<More characters are coming!!!>");
                        }
                    }
                    else {
                        isDrawSkel = false;
                        isDrawAttachedSkin = false;
                        isDrawOriginalSkin = false;
                    }
                    // Slider box for camera
                    ImGui::Text("\nCamera Settings");
                    ImGui::SliderFloat("Distance", &(Window::Cam->Distance), 1.0f, 100.0f);
                    ImGui::SliderFloat("Azimuth", &(Window::Cam->Azimuth), 0.0f, 360.0f);
                    ImGui::SliderFloat("Incline", &(Window::Cam->Incline), -90.0f, 90.0f);
                }
            }

            
            

            ImGui::End();
        }

        // Idle callback. Updating objects, etc. can be done here.
        Window::idleCallback();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    Window::cleanUp();
    // Destroy the window.
    glfwDestroyWindow(window);
    // Terminate GLFW.
    glfwTerminate();

    exit(EXIT_SUCCESS);
}