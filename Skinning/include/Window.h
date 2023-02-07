#pragma once

#include "Camera.h"
#include "Skeleton.h"
#include "Shader.h"
#include "core.h"
#include "Skin.h"

class Window {
public:
    // Window Properties
    static int width;
    static int height;
    static const char* windowTitle;

    // Objects to render
    static Skeleton* testSkel;
    static Skeleton* waspSkel;
    static Skeleton* dragonSkel;
    static Skeleton* currSkel;
    static Skin* waspSkin;
    static Skin* currSkin;

    // Camera
    static Camera* Cam;

    // Shader Program
    static ShaderProgram* shaderProgram;

    // Act as Constructors and desctructors
    static bool initializeObjects();
    static bool initializeProgram();
    static void cleanUp();

    // for the Window
    static GLFWwindow* createWindow(int width, int height);
    static void resizeCallback(GLFWwindow* window, int width, int height);
    
    // for loading model
    static void setSkel(GLFWwindow* window, const char* skelName);

    // for attaching skin
    static void setSkin(GLFWwindow* window, const char* skinName);

    // update and draw functions
    static void idleCallback();
    static void displayCallback(GLFWwindow* window, bool isDrawOriginalSkin, bool isDrawSkel, bool isDrawAttachedSkin);

    // helper to reset the camera
    static void resetCamera();

    // callbacks - for interaction
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    //static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
    //static void cursor_callback(GLFWwindow* window, double currX, double currY);
};