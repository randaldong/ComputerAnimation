#pragma once
#include "Particle.hpp"

class Spring
{
public:
    Particle* p1, *p2;
    double restLen; // Spring rest length L0;
    // Hooke coefficient Ks, the spring constant describing the <stiffness> of the spring
    // Damping constant Kd, representing the resistence of moving close
    // Should be specified according to spring types: strctural, shear, or bending
    double Ks;
    double Kd;

    Spring(Particle* p1Arg, Particle* p2Arg, double stiffness, double damping)
    {
        p1 = p1Arg;
        p2 = p2Arg;
        restLen = vec3::dist(p1->position, p2->position);
        Ks = stiffness;
        Kd = damping;
    }

    void applyForce()
    {
        double currLen = vec3::dist(p1->position, p2->position);
        vec3 f1Dir = vec3::getNormalized(p2->position - p1->position); // direction of force on p1;
        double vCloseNeg = vec3::dot((p2->velocity - p1->velocity), f1Dir);
        vec3 f1 = f1Dir * (Ks * (currLen - restLen) + Kd * vCloseNeg);
        p1->applyForce(f1);
        p2->applyForce(f1.negative());
    }
};
