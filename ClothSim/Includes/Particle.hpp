#pragma once
#include "Utils.hpp"

struct Vertex  // for ground & sphere which are fixed
{
    vec3 position;
    vec3 normal;
    vec2 texCoord;

    Vertex(vec3 pos) { 
        position = pos;
        normal.setAsZero();
    }
};

class Particle
{
public:
    double mass;
    bool isFixed;
    vec3 position;
    vec3 initPosition;
    vec3 normal;
    vec2 texCoord;
    vec3 force;
    vec3 acceleration;
    vec3 velocity;

    Particle(vec3 pos)
    {
        mass = 1.0;
        isFixed = false;
        position = pos;
        initPosition = pos;
        normal.setAsZero();
        force.setAsZero();
        acceleration.setAsZero();
        velocity.setAsZero();
    }

    void applyForce(vec3 forceArg) { force += forceArg; }

    void integrateMotion(double deltaT)
    {
        if (!isFixed) { // only update unfixed particles
            acceleration = force / mass;
            velocity += acceleration * deltaT;
            position += velocity * deltaT;
        }
        force.setAsZero(); // after applying forces, it becomes zero and need updated next frame
    }

    void reset()
    {
        position = initPosition;
        force.setAsZero();
        acceleration.setAsZero();
        velocity.setAsZero();
    }
};
