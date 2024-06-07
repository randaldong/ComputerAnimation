#define STB_IMAGE_IMPLEMENTATION
#include "Cloth.hpp"
#include "Display.hpp"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
#include "../imgui/imgui_internal.h"
////////////////////////////////////////////////////////////////////////////////
// Global parameters/objects setting
////////////////////////////////////////////////////////////////////////////////

/** Cloth **/
const char* clothTexFilename1 = "assets/clothTex1.jpg";
const char* clothTexFilename2 = "assets/clothTex2.jpg";
const char* clothTexFilename3 = "assets/clothTex3.jpg";
const char* clothTexFilename4 = "assets/clothTex4.jpg";
const char* clothTexFilename5 = "assets/clothTex5.jpg";
const char* clothTexFilename6 = "assets/clothTex6.jpg";
const char* pinMode1 = "Pin Upper Corner";
const char* pinMode2 = "Pin Upper Corner";
vec3 clothPos(-3, 8, -2);
vec2 clothSize(6, 6);
Cloth cloth(clothPos, clothSize, pinMode1);
float clothMoveSpeed = 0.05f;

/** Environment **/
// rigid-ground
const char* groundTexFilename = "assets/groundTex1.jpg";
vec3 groundPos(-5, 1, 2);
vec2 groundSize(10, 10);
float reGround = 0.6f;
Ground ground(groundPos, groundSize, reGround);
// rigid-sphere, magic_orb
const char* orbTexFilename = "assets/orbTex1.jpg";
vec3 orbCenter(0, 4, -2);
float orbRadius = 1.5f;
float reOrb = 0.8f;
Sphere orb(orbRadius, orbCenter, reOrb);
bool isOrbUseColor = false;
bool isOrbTransparent = true;
// sphere for light
const char* lightTexFilename = "assets/lightTex.jpg";
float bulbRadius = 0.1f;
Sphere lightBulb(bulbRadius, vec3(pointLight.pos.x, pointLight.pos.y, pointLight.pos.z));
float lightMoveSpeed = 0.05f;
bool isLightUseColor = true;
bool isLightTransparent = false;
// sphere for wind/fan
const char* fanTexFilename = "assets/fanTex.jpg";
vec3 fanPos = vec3(2, 7, 3);
float fanRadius = 0.3f;
Sphere fan(fanRadius, vec3(fanPos.x, fanPos.y, fanPos.z));
float fanMoveSpeed = 0.05f;
bool isFanUseColor = false;
bool isFanTransparent = true;
// wind
double airDensity = 1.255;
double airDragCoeff = 1.28; 
float vWindVal = -0.0001;
vec3 windDir = cloth.clothPos - fanPos;
vec3 vWind = vWindVal * windDir;

/** User Mouse Grab **/
double grabForceVal = 15;
vec3 grabStartPos;
vec3 grabDir;
vec3 grabForce;

/** Window & World **/
// Simulation
int simFreq = 30; // simulation times per frame, gravity should be divided by this
double deltaT = 0.01;
// window1
const char* windowIconFile = "assets/windowIcon1.png";
const char* windowTitle = "Randal's Magic Cloth";
int windowWidth = 1920;
int windowHeight = 1440;
glm::vec4 bgColor = { 20.0f / 255, 20.0f / 255, 50.0f / 255, 0.95f };
GLFWwindow* window;
//// window2
//const char* windowIconFile2 = "Assets/windowIcon2.png";
//const char* windowTitle2 = "Control Panel";
//int windowWidth2 = 600;
//int windowHeight2 = 1200;
//glm::vec4 bgColor2 = { 20.0f / 255, 20.0f / 255, 50.0f / 255, 0.95f };
//GLFWwindow* window2;
vec3 gravity(0.0, -9.8 / simFreq, 0.0);

/** ImGUI Panel **/
bool isPaused = false;
bool isMovingCloth = false; // Move cloth
bool isMovingCamera = false; // Move camera
bool isMovingLight = false; // Move light
bool isMovingWind = false; // Move wind spawn/fan
int clickWindHowling = 0; // Turn on the fan
bool isWindHowling = false;
bool isGrabAllowed = false; // if the checkbox is active
bool isGrabing = false; // if mouse left button is down

////////////////////////////////////////////////////////////////////////////////
// Functions & callbacks declaration
////////////////////////////////////////////////////////////////////////////////

GLFWwindow* createWindow(const char* windowTitle, const char* windowIconFile, int width, int height);
// object movement keymaps
void moveClothKeymap(GLFWwindow* window);
void moveCameraKeymap(GLFWwindow* window);
void moveLightKeymap(GLFWwindow* window);
void moveWindKeymap(GLFWwindow* window);
// imgui style
void setImGuiStyle();
// glfw callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void global_key_call_back(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);

////////////////////////////////////////////////////////////////////////////////
// main() function
////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char* argv[])
{
    /** -------------------------------- Preparation -------------------------------- **/
    // Initialize GLFW
    glfwInit();
    // Set OpenGL version number as 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Use the core profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /** -------------------------------- Create GLFW windows -------------------------------- **/
    /** window **/
    window = createWindow(windowTitle, windowIconFile, windowWidth, windowHeight);
    glfwMakeContextCurrent(window);
    // Initialize GLAD : load all OpenGL function pointers
    // this should be done before using any openGL function and after setting the current context
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD." << std::endl;
        return -1;
    }

    /** Setup Dear ImGui context **/
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL;
    io.LogFilename = NULL;
    io.WantSaveIniSettings = false;
    // Load Icons and Fonts
    //io.Fonts->AddFontFromFileTTF("assets/HFSprouts-2.ttf", 35.0f);
    io.Fonts->AddFontFromFileTTF("Assets/Ancoa-Demo-Bold-2.otf", 40.0f);

    /** Register callback functions **/
    // Callback functions should be registered after creating window and before initializing render loop
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, global_key_call_back);

    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Create Renderers
    ClothRenderer clothRenderer1(&cloth, clothTexFilename1);
    ClothRenderer clothRenderer2(&cloth, clothTexFilename2);
    ClothRenderer clothRenderer3(&cloth, clothTexFilename3);
    ClothRenderer clothRenderer4(&cloth, clothTexFilename4);
    ClothRenderer clothRenderer5(&cloth, clothTexFilename5);
    ClothRenderer clothRenderer6(&cloth, clothTexFilename6);
    ClothRenderer currClothRenderer = clothRenderer1;
    int currClothTex = 1;
    GroundRenderer groundRenderer(&ground, groundTexFilename);
    SphereRenderer orbRenderer(&orb, orbTexFilename, isOrbUseColor, isOrbTransparent);
    SphereRenderer lightRenderer(&lightBulb, lightTexFilename, isLightUseColor, isLightTransparent);
    SphereRenderer fanRenderer(&fan, fanTexFilename, isFanUseColor, isFanTransparent);

    glEnable(GL_DEPTH_TEST);
    glPointSize(3);

    /** -------------------------------- Rendering loop -------------------------------- **/
    while (!glfwWindowShouldClose(window))
    {
        /** Check for events **/
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        setImGuiStyle();

        {
            ImGui::Begin("Control Panel");
            // FPS
            ImGui::Text("Simulate with %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            /** General **/
            // Pause
            ImGui::Checkbox("Pause Simulation", &isPaused);
            // Reset
            bool isResetPushed = ImGui::Button("Reset", ImVec2(100, 60));
            if (isResetPushed) { 
                cloth.reset(); 
            }

            /** Cloth **/
            ImGui::Text("\nCloth");
            // move cloth
            ImGui::Checkbox("Move Cloth", &isMovingCloth);
            if (isMovingCloth) {
                isMovingCamera = false;
                isMovingLight = false;
                isMovingWind = false;
                isGrabAllowed = false;
                moveClothKeymap(window);
            }
            // load different texture
            static int selectedTex = 0;
            std::vector<const char*> textures = { 
                "The Starry Night",
                "Allegory of Sight", 
                "World of Warcraft",
                "The Witcher",
                "God of War",
                "Genshin Impact",
            };
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5);
            ImGui::Combo("Change Texture", &selectedTex, textures.data(), textures.size());
            ImGui::PopItemWidth();
            if (textures[selectedTex] == "The Starry Night" && currClothTex != 1) {
                currClothRenderer = clothRenderer1;
                currClothTex = 1;
            }
            if (textures[selectedTex] == "Allegory of Sight" && currClothTex != 2) {
                currClothRenderer = clothRenderer2;
                currClothTex = 2;
            }
            if (textures[selectedTex] == "World of Warcraft" && currClothTex != 3) {
                currClothRenderer = clothRenderer3;
                currClothTex = 3;
            }
            if (textures[selectedTex] == "The Witcher" && currClothTex != 4) {
                currClothRenderer = clothRenderer4;
                currClothTex = 4;
            }
            if (textures[selectedTex] == "God of War" && currClothTex != 5) {
                currClothRenderer = clothRenderer5;
                currClothTex = 5;
            }
            if (textures[selectedTex] == "Genshin Impact" && currClothTex != 6) {
                currClothRenderer = clothRenderer6;
                currClothTex = 6;
            }
            // change pin mode
            static int selectedPinMode = 0;
            std::vector<const char*> pinModes = {
                "Pin Upper Corner",
                "Pin Upper Edge",
                "Drop Cloth",
            };
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5);
            ImGui::Combo("Change Pin Mode", &selectedPinMode, pinModes.data(), pinModes.size());
            ImGui::PopItemWidth();
            if (pinModes[selectedPinMode] == "Pin Upper Corner" && cloth.pinMode != "Pin Upper Corner") {
                cloth.pinMode = "Pin Upper Corner";
                cloth.updatePinMode();
            }
            if (pinModes[selectedPinMode] == "Pin Upper Edge" && cloth.pinMode != "Pin Upper Edge") {
                cloth.pinMode = "Pin Upper Edge";
                cloth.updatePinMode();
            }
            if (pinModes[selectedPinMode] == "Drop Cloth" && cloth.pinMode != "Drop Cloth") {
                cloth.dropCloth();
            }
            // change draw mode
            static int selectedDrawMode = 2;
            std::vector<const char*> drawModes = {
                "Draw Particles",
                "Draw Springs",
                "Draw Cloth",
            };
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5);
            ImGui::Combo("Change Draw Mode", &selectedDrawMode, drawModes.data(), drawModes.size());
            ImGui::PopItemWidth();
            if (drawModes[selectedDrawMode] == "Draw Particles" && cloth.drawMode != Cloth::DRAW_PARTICLES) {
                cloth.drawMode = Cloth::DRAW_PARTICLES;
            }
            if (drawModes[selectedDrawMode] == "Draw Springs" && cloth.drawMode != Cloth::DRAW_SPRINGS) {
                cloth.drawMode = Cloth::DRAW_SPRINGS;
            }
            if (drawModes[selectedDrawMode] == "Draw Cloth" && cloth.drawMode != Cloth::DRAW_CLOTH) {
                cloth.drawMode = Cloth::DRAW_CLOTH;
            }
            // change spring parameters
            ImGui::Text("Hooke coefficient Ks");
            ImGui::SliderFloat("structural Ks", &(cloth.structuralStiffness), 500.0f, 5000.0f);
            ImGui::SliderFloat("shearing Ks", &(cloth.shearStiffness), 20.0f, 500.0f);
            ImGui::SliderFloat("bending Ks", &(cloth.bendingStiffness), 100.0f, 1000.0f);
            ImGui::Text("Damping constant Kd");
            ImGui::SliderFloat("structural Kd", &(cloth.structuralDamping), 10.0f, 100.0f);
            ImGui::SliderFloat("shearing Kd", &(cloth.shearDamping), 0.0f, 5.0f);
            ImGui::SliderFloat("bending Kd", &(cloth.bendingDamping), 0.0f, 50.0f);
            // friction
            ImGui::Text("\nFriction");
            ImGui::SliderFloat("cloth @ orb", &(orb.restitution), 0.0f, 1.0f);
            ImGui::SliderFloat("cloth @ ground", &(ground.restitution), 0.0f, 1.0f);

            /** Light **/
            ImGui::Text("\nLight");
            // move light
            ImGui::Checkbox("Move Light", &isMovingLight);
            if (isMovingLight) {
                isMovingCloth = false;
                isMovingCamera = false;
                isMovingWind = false;
                isGrabAllowed = false;
                moveLightKeymap(window);
            }
            // change light color
            ImGui::ColorEdit3("light color", &pointLight.color[0], ImGuiColorEditFlags_PickerHueWheel);

            /** Wind **/
            ImGui::Text("\nWind");
            // move wind spawn
            ImGui::Checkbox("Move Wind Spawn", &isMovingWind);
            if (isMovingWind) {
                isMovingCloth = false;
                isMovingCamera = false;
                isMovingLight = false;
                isGrabAllowed = false;
                moveWindKeymap(window);
            }
            // wind's howling
            if (ImGui::Button("Wind's Howling", ImVec2(250, 60))) {
                if (clickWindHowling == 0) clickWindHowling += 1;
            }
            else {
                ImGui::SameLine();
                if (ImGui::Button("Wind's Calm", ImVec2(250, 60))) { 
                    if (clickWindHowling > 0) clickWindHowling -= 1;
                }
            }
            if (clickWindHowling > 0) {
                ImGui::SliderFloat("wind force", &(vWindVal), 0.0001f, 20.0f);
            }
            else {
                vWindVal = -0.0001;

            }
            // update wind
            windDir = cloth.clothPos - fanPos;
            vWind = vWindVal * windDir;

            /** User Grab **/
            ImGui::Text("\nMouse Grab");
            ImGui::Checkbox("Activate Grab Mode", &isGrabAllowed);
            if (isGrabAllowed) {
                isMovingCloth = false;
                isMovingLight = false;
                isMovingWind = false;
                isMovingCamera = false;
                ImGui::Text("Grabing the cloth...");
            }

            /** Camera **/
            ImGui::Text("\nCamera");
            // move camera
            ImGui::Checkbox("Move Camera", &isMovingCamera);
            if (isMovingCamera) {
                isMovingCloth = false;
                isMovingLight = false;
                isMovingWind = false;
                isGrabAllowed = false;
                moveCameraKeymap(window);
            }

            ImGui::End();
        }
        
        ImGui::Render();

        /** Set background clolor **/
        glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0); // Set color value (R,G,B,A) - Set Status
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /** Simulation **/
        if (!isPaused) {
            for (int i = 0; i < simFreq; i++) {
                cloth.computeForces(gravity, airDensity, airDragCoeff, vWind);
                cloth.integrateMotion(deltaT);
                cloth.collisionResponse(&ground, &orb);
            }
            cloth.updateNormal();
        }

        /** Rendering **/
        currClothRenderer.Update();
        orbRenderer.Update();
        groundRenderer.Update();
        lightRenderer.Update();
        fanRenderer.Update();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////
// Functions & callbacks definiation
////////////////////////////////////////////////////////////////////////////////

GLFWwindow* createWindow(const char* windowTitle, const char* windowIconFile, int width, int height)
{
    GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
    }
    // initial camera
    camera.Aspect = float(width) / float(height);
    // resize
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // load image for icon
    int IMGwidth, IMGheight, IMGchannels;
    unsigned char* IMGpixels = stbi_load(windowIconFile, &IMGwidth, &IMGheight, &IMGchannels, 4);
    // change window icon
    GLFWimage images[1];
    images[0].width = IMGwidth;
    images[0].height = IMGheight;
    images[0].pixels = IMGpixels;
    glfwSetWindowIcon(window, 1, images);

    return window;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    camera.Aspect = float(width) / float(height);
}

void global_key_call_back(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    /** Keyboard control **/ // If key did not get pressed it will return GLFW_RELEASE
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (isGrabAllowed) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && (!isPaused)) { // Start wind
            isGrabing = true;
            // Set start point of grab direction
            grabStartPos.setAsZero();
            glfwGetCursorPos(window, &grabStartPos.x, &grabStartPos.y);
            grabStartPos.y = -grabStartPos.y; // Reverse y since the screen local in the fourth quadrant
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE && (!isPaused)) { // End wind
            isGrabing = false;
            grabDir.setAsZero();
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && (!isPaused)) { // inactivate grab mode
            isGrabAllowed = false;
        }
    }
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (isGrabAllowed) {
        if (isGrabing && (!isPaused)) {
            grabDir = vec3(xpos, -ypos, 0) - grabStartPos;
            grabDir.normalize();
            grabForce = grabDir * grabForceVal;
            cloth.addGrabForce(grabForce);
        }
    }
}

void moveClothKeymap(GLFWwindow* window)
{
    /** Cloth control : [W] [S] [A] [D] [Q] [E] [Z] [C]**/
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && (!isPaused)) {
        cloth.transform("Up", clothMoveSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && (!isPaused)) {
        cloth.transform("Down", clothMoveSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && (!isPaused)) {
        cloth.transform("Left", clothMoveSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && (!isPaused)) {
        cloth.transform("Right", clothMoveSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && (!isPaused)) {
        cloth.transform("Inward", clothMoveSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && (!isPaused)) {
        cloth.transform("Outward", clothMoveSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && (!isPaused)) {
        cloth.transform("Rotate Counterclockwise", clothMoveSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && (!isPaused)) {
        cloth.transform("Rotate Clockwise", clothMoveSpeed);
    }
}

void moveCameraKeymap(GLFWwindow* window)
{
    /** Camera control : [W] [S] [A] [D] [Q] [E] **/
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.cameraPos.y += camera.moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.cameraPos.y -= camera.moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.cameraPos.x -= camera.moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.cameraPos.x += camera.moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera.cameraPos.z -= camera.moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.cameraPos.z += camera.moveSpeed;
    }
}

void moveLightKeymap(GLFWwindow* window)
{
    /** Camera control : [W] [S] [A] [D] [Q] [E] **/
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        lightBulb.translate("Up", lightMoveSpeed);
        pointLight.pos.y += lightMoveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        lightBulb.translate("Down", lightMoveSpeed);
        pointLight.pos.y -= lightMoveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        lightBulb.translate("Left", lightMoveSpeed);
        pointLight.pos.x -= lightMoveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        lightBulb.translate("Right", lightMoveSpeed);
        pointLight.pos.x += lightMoveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        lightBulb.translate("Inward", lightMoveSpeed);
        pointLight.pos.z -= lightMoveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        lightBulb.translate("Outward", lightMoveSpeed);
        pointLight.pos.z += lightMoveSpeed;
    }
}

void moveWindKeymap(GLFWwindow* window)
{
    /** Wind Spwan control : [W] [S] [A] [D] [Q] [E] **/
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        fan.translate("Up", fanMoveSpeed);
        fanPos.y += fanMoveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        fan.translate("Down", fanMoveSpeed);
        fanPos.y -= fanMoveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        fan.translate("Left", fanMoveSpeed);
        fanPos.x -= fanMoveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        fan.translate("Right", fanMoveSpeed);
        fanPos.x += fanMoveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        fan.translate("Inward", fanMoveSpeed);
        fanPos.z -= fanMoveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        fan.translate("Outward", fanMoveSpeed);
        fanPos.z += fanMoveSpeed;
    }
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
    style.Colors[ImGuiCol_TitleBgActive] = ImColor(128, 128, 255, 0.6 * 255);
    //style.Colors[ImGuiCol_TitleBgCollapsed] = ImColor(180, 200, 255, 0.5 * 255);
}
