////////////////////////////////////////
// Camera.cpp
////////////////////////////////////////

#include "Camera.h"

Camera::Camera(int m) {
    mode = m;
    Reset();
}
void Camera::Update() {
    // Compute camera world matrix
    glm::mat4 world(1);
    world[3][2] = Distance;
    world = glm::eulerAngleY(glm::radians(-Azimuth)) * glm::eulerAngleX(glm::radians(-Incline)) * world;

    // Compute view matrix (inverse of world matrix)
    glm::mat4 view = glm::inverse(world);

    // Compute perspective projection matrix
    glm::mat4 project = glm::perspective(glm::radians(FOV), Aspect, NearClip, FarClip);

    // Compute final view-projection matrix
    ViewProjectMtx = project * view;
}
void Camera::Reset() {
    FOV = 45.0f;
    Aspect = 1.33f;
    NearClip = 0.1f;
    FarClip = 100.0f;
    // Adjust distance, azimuth, incline according to different models.
    // Each model has its own camera mode.
    switch (mode) {
    case 1:
        Distance = 2.2f;
        Azimuth = 154.0f;
        Incline = 19.0f;
        break;
    case 2:
        Distance = 3.7f;
        Azimuth = 202.0f;
        Incline = 26.0f;
        break;
    case 3:
        Distance = 43.0f;
        Azimuth = 300.0f;
        Incline = 24.0f;
        break;
    }
}