#pragma once

#include "Camera.h"
#include "Skeleton.h"
#include "Shader.h"
#include "core.h"
#include "Skin.h"
#include "AnimationPlayer.h"
#include "AnimRig.h"

class Window {
public:
    // Window Properties
    static int width;
    static int height;
    static const char* windowTitle;

    // Objects to render
    static Skeleton* testSkel;
    static Skeleton* wasp1Skel;
    static Skeleton* dragonSkel;
    static Skeleton* currSkel;

    static Skin* wasp1Skin;
    static Skin* currSkin;

    static AnimRig* waspRig;
    static AnimationClip* waspClip;
    static AnimationPlayer* waspPlayer;
    static AnimationPlayer* currPlayer;

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

    // for playing animation
    static void setAnimRig(GLFWwindow* window, const char* animRigName);

    // update and draw functions
    static void idleCallback();
    static void displayCallback(GLFWwindow* window, bool isDrawOriginalSkin, bool isDrawSkel, bool isDrawAttachedSkin, bool isPlayAnim);

    // helper to reset the camera
    static void resetCamera();

    // callbacks - for interaction
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    //static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
    //static void cursor_callback(GLFWwindow* window, double currX, double currY);
};