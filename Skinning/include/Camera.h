////////////////////////////////////////
// Camera.h
////////////////////////////////////////

#pragma once

#include "core.h"
#include "glm/gtx/euler_angles.hpp"

// The Camera class provides a simple means to controlling the 3D camera. It could
// be extended to support more interactive controls. Ultimately. the camera sets the
// GL projection and viewing matrices.

class Camera {
public:
    Camera(int m = 1);

    void Update();
    void Reset();

    const glm::mat4 &GetViewProjectMtx() { return ViewProjectMtx; }

    // Viewing mode, change according to different models
    int mode;

    // Perspective controls
    float FOV;       // Field of View Angle (degrees)
    float Aspect;    // Aspect Ratio
    float NearClip;  // Near clipping plane distance
    float FarClip;   // Far clipping plane distance

    // Polar controls
    float Distance;  // Distance of the camera eye position to the origin (meters)
    float Azimuth;   // Rotation of the camera eye position around the Y axis (degrees)
    float Incline;   // Angle of the camera eye position over the XZ plane (degrees)

    // Computed data
    glm::mat4 ViewProjectMtx;
};