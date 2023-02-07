#include "Window.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "Randal's Renderer";

// Objects to render
Skeleton* Window::testSkel;
Skeleton* Window::wasp1Skel;
Skeleton* Window::wasp2Skel;
Skeleton* Window::dragonSkel;
Skeleton* Window::currSkel;
Skin* Window::wasp1Skin;
Skin* Window::wasp2Skin;
Skin* Window::currSkin;

// Camera Properties
Camera* Window::Cam;

// Interaction Variables
bool LeftDown, RightDown;

// The shader program
ShaderProgram* Window::shaderProgram;

// Constructors and desctructors
bool Window::initializeProgram() {
    // Create a shader program
    shaderProgram = new ShaderProgram("shaders/shader.glsl");

    // Check the shader program.
    if (!shaderProgram) {
        std::cerr << "Failed to initialize shader program" << std::endl;
        return false;
    }
    return true;
}

bool Window::initializeObjects() {
    // Create a skeleton
    testSkel = new Skeleton();
    wasp1Skel = new Skeleton();
    wasp2Skel = new Skeleton();
    dragonSkel = new Skeleton();
    testSkel->Load();
    wasp1Skel->Load("assets/wasp1.skel");
    wasp2Skel->Load("assets/wasp2.skel");
    dragonSkel->Load("assets/dragon.skel");
    currSkel = testSkel;
    currSkel->Update(glm::mat4(1.0f)); // Update to get the world matrix of every joint
    // Create a skin
    wasp1Skin = new Skin(wasp1Skel);
    wasp1Skin->Load("assets/wasp1.skin");
    wasp2Skin = new Skin(wasp2Skel);
    wasp2Skin->Load("assets/wasp2.skin");
    currSkin = wasp1Skin;
    currSkin->Update();

    return true;
}

// for the Window
GLFWwindow* Window::createWindow(int width, int height) {
    // Initialize GLFW.
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return NULL;
    }

    // 4x antialiasing.
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create the GLFW window.
    GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

    // Check if the window could not be created.
    if (!window) {
        std::cout << "An error has devoured the window :< do not blame the developer =.=" << std::endl;
        glfwTerminate();
        return NULL;
    }

    // Make the context of the window.
    glfwMakeContextCurrent(window);

    // load image for icon
    int IMGwidth, IMGheight, IMGchannels;
    unsigned char* IMGpixels = stbi_load("assets/icon.png", &IMGwidth, &IMGheight, &IMGchannels, 4);
    // change window icon
    GLFWimage images[1];
    images[0].width = IMGwidth;
    images[0].height = IMGheight;
    images[0].pixels = IMGpixels;
    glfwSetWindowIcon(window, 1, images);

    // Initialize GLEW
    glewInit();

    // Set swap interval to 1.
    glfwSwapInterval(0);

    // set up the camera
    // test: m = 1; wasp: m = 2; dragon: m = 3
    int m;
    if (!currSkel || currSkel == testSkel) m = 1;
    else if (currSkel == wasp1Skel || currSkel == wasp2Skel) m = 2;
    else if (currSkel == dragonSkel) m = 3;
    Cam = new Camera();
    Cam->Aspect = float(width) / float(height);

    // initialize the interaction variables
    //LeftDown = RightDown = false;
    //MouseX = MouseY = 0;

    // Call the resize callback to make sure things get drawn immediately.
    Window::resizeCallback(window, width, height);

    return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height) {
    Window::width = width;
    Window::height = height;
    // Set the viewport size.
    glViewport(0, 0, width, height);

    Cam->Aspect = float(width) / float(height);
}

// update and draw functions
void Window::idleCallback() {
    // Perform any updates as necessary.
    Cam->Update();
    testSkel->Update(glm::mat4(1.0f));
    wasp1Skel->Update(glm::mat4(1.0f));
    wasp2Skel->Update(glm::mat4(1.0f));
    dragonSkel->Update(glm::mat4(1.0f));
    wasp1Skin->Update();
    wasp2Skin->Update();
}

void Window::displayCallback(GLFWwindow* window, bool isDrawOriginalSkin, bool isDrawSkel, bool isDrawAttachedSkin) {
    // Render the object.
    if (isDrawSkel) {
        currSkel->Draw(Cam->GetViewProjectMtx(), Window::shaderProgram->programID);
    }
    else if (isDrawAttachedSkin) {
        currSkin->Draw(isDrawOriginalSkin, Cam->GetViewProjectMtx(), Window::shaderProgram->programID);
    }
    else if (isDrawOriginalSkin) {
        currSkin->Draw(isDrawOriginalSkin, Cam->GetViewProjectMtx(), Window::shaderProgram->programID);
    }
    // Gets events, including input such as keyboard and mouse or window resizing.
    // Move to main due to ImGui
    // glfwPollEvents();
    // Swap buffers.
    // Move to main due to ImGui
    // glfwSwapBuffers(window);
}

// helper to reset the camera
void Window::resetCamera() {
    Cam->Reset();
    Cam->Aspect = float(Window::width) / float(Window::height);
    currSkel->root->ResetAll();
}

void Window::setSkel(GLFWwindow* window, const char* skelName) {
    if (skelName == "test") {
        currSkel = testSkel;
        Cam->mode = 1;
    }
    else if (skelName == "wasp1") {
        currSkel = wasp1Skel;
        Cam->mode = 2;
    }
    else if (skelName == "wasp2") {
        currSkel = wasp2Skel;
        Cam->mode = 2;
    }
    else if (skelName == "dragon") {
        currSkel = dragonSkel;
        Cam->mode = 3;
    }
}

void Window::setSkin(GLFWwindow* window, const char* skinName)
{
    if (skinName == "wasp1") {
        currSkin = wasp1Skin;
    }
    if (skinName == "wasp2") {
        currSkin = wasp2Skin;
    }
}


// callbacks - for Interaction
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Check for a key press.
    if (action == GLFW_PRESS) {
        const float rate = 10.0f;
        switch (key) {
            case GLFW_KEY_ESCAPE:
                // Close the window. This causes the program to also terminate.
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case GLFW_KEY_R:
                resetCamera();
                break;
        }
    }
}

void Window::cleanUp() {
    // Deallcoate the objects.
    delete testSkel;
    delete wasp1Skel;
    delete wasp2Skel;
    delete dragonSkel;
    delete wasp1Skin;
    delete wasp2Skin;
    delete Cam;

    // Delete the shader program.
    glDeleteProgram(shaderProgram->programID);
    delete shaderProgram;
}

//void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods) {
//    if (button == GLFW_MOUSE_BUTTON_LEFT) {
//        LeftDown = (action == GLFW_PRESS);
//    }
//    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
//        RightDown = (action == GLFW_PRESS);
//    }
//}

//void Window::cursor_callback(GLFWwindow* window, double currX, double currY) {
//    int maxDelta = 100;
//    int dx = glm::clamp((int)currX - MouseX, -maxDelta, maxDelta);
//    int dy = glm::clamp(-((int)currY - MouseY), -maxDelta, maxDelta);
//
//    MouseX = (int)currX;
//    MouseY = (int)currY;
//
//    // Move camera
//    // NOTE: this should really be part of Camera::Update()
//    if (LeftDown) {
//        const float rate = 1.0f;
//        Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
//        Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
//    }
//    if (RightDown) {
//        const float rate = 0.005f;
//        float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
//        Cam->SetDistance(dist);
//    }
//}