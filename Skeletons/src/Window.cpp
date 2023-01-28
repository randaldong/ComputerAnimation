#include "Window.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "Randal's Gentle Observer under Gravity, Sunlight, Magnetism";

// Objects to render
Skeleton* Window::test;
Skeleton* Window::wasp;
Skeleton* Window::dragon;
Skeleton* Window::currSkel;

// Camera Properties
Camera* Cam;

// Interaction Variables
bool LeftDown, MiddleDown, RightDown;
int MouseX, MouseY;

// The shader program id
GLuint Window::shaderProgram;

// Constructors and desctructors
bool Window::initializeProgram() {
    // Create a shader program with a vertex shader and a fragment shader.
    shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

    // Check the shader program.
    if (!shaderProgram) {
        std::cerr << "Failed to initialize shader program" << std::endl;
        return false;
    }

    return true;
}

bool Window::initializeObjects() {
    // Create a skeleton
    test = new Skeleton();
    wasp = new Skeleton();
    dragon = new Skeleton();
    test->Load();
    wasp->Load("assets/wasp.skel");
    dragon->Load("assets/dragon.skel");
    currSkel = test;
    return true;
}

void Window::cleanUp() {
    // Deallcoate the objects.
    delete test;
    delete wasp;
    delete dragon;

    // Delete the shader program.
    glDeleteProgram(shaderProgram);
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
    if (!currSkel || currSkel == test) m = 1;
    else if (currSkel == wasp) m = 2;
    else if (currSkel == dragon) m = 3;
    Cam = new Camera(m);
    Cam->SetAspect(float(width) / float(height));

    // initialize the interaction variables
    LeftDown = RightDown = false;
    MouseX = MouseY = 0;

    // Call the resize callback to make sure things get drawn immediately.
    Window::resizeCallback(window, width, height);

    return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height) {
    Window::width = width;
    Window::height = height;
    // Set the viewport size.
    glViewport(0, 0, width, height);

    Cam->SetAspect(float(width) / float(height));
}

// update and draw functions
void Window::idleCallback() {
    // Perform any updates as necessary.
    Cam->Update();

    test->Update(glm::mat4(1.0f));
    wasp->Update(glm::mat4(1.0f));
    dragon->Update(glm::mat4(1.0f));
}

void Window::displayCallback(GLFWwindow* window) {
    // Clear the color and depth buffers.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the object.
    currSkel->Draw(Cam->GetViewProjectMtx(), Window::shaderProgram);

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
    Cam->SetAspect(float(Window::width) / float(Window::height));
}

// callbacks - for Interaction
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    /*
     * TODO: Modify below to add your key callbacks.
     */

    // Check for a key press.
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                // Close the window. This causes the program to also terminate.
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case GLFW_KEY_R:
                resetCamera();
                break;
            case GLFW_KEY_1:
                currSkel = test;
                Cam->mode = 1;
                resetCamera();

                break;
            case GLFW_KEY_2:
                currSkel = wasp;
                Cam->mode = 2;
                resetCamera();

                break;
            case GLFW_KEY_3:
                currSkel = dragon;
                Cam->mode = 3;
                resetCamera();

                break;
            default:
                break;
        }
    }
}

void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        LeftDown = (action == GLFW_PRESS);
    }
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        MiddleDown = (action == GLFW_PRESS);
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        RightDown = (action == GLFW_PRESS);
    }
}

void Window::cursor_callback(GLFWwindow* window, double currX, double currY) {
    int maxDelta = 100;
    int dx = glm::clamp((int)currX - MouseX, -maxDelta, maxDelta);
    int dy = glm::clamp(-((int)currY - MouseY), -maxDelta, maxDelta);

    MouseX = (int)currX;
    MouseY = (int)currY;

    // Move camera
    // NOTE: this should really be part of Camera::Update()
    if (LeftDown) {
        const float rate = 1.0f;
        Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
        Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
    }
    if (RightDown) {
        const float rate = 0.005f;
        float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
        Cam->SetDistance(dist);
    }
}