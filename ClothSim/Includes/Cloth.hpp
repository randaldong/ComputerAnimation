#pragma once
#include "Spring.hpp"
#include "Rigid.hpp"

class Cloth
{
public:
    int width, height;
    int numCols, numRows;
    int particleDensity = 5; // # particles per unit, control cloth pattern resolution
    const char* pinMode;
    float structuralStiffness = 1000.0;
    float shearStiffness = 50.0;
    float bendingStiffness = 400.0;
    float structuralDamping = 12.0;
    float shearDamping = 0.6;
    float bendingDamping = 5.0;
    vec3 clothPos;
    std::vector<Particle*> particles;
    std::vector<Spring*> springs;
    std::vector<Particle*> trianglePs; // particles that make up of cloth triangles

    enum DrawModeEnum {
        DRAW_PARTICLES,
        DRAW_SPRINGS,
        DRAW_CLOTH,
    };
    DrawModeEnum drawMode = DRAW_CLOTH;

    Cloth(vec3 pos, vec2 size, const char* pinModeArg)
    {
        clothPos = pos;
        width = size.x;
        height = size.y;
        numCols = width * particleDensity;
        numRows = height * particleDensity;
        pinMode = pinModeArg;
        init();
    }

    Particle* getParticle(int row, int col) { return particles[row * numCols + col]; }

    void PinParticle(int row, int col, vec3 offset)
    {
        getParticle(row, col)->position = vec3(((double)col / particleDensity), (double)row / particleDensity, 0) + offset;
        getParticle(row, col)->isFixed = true;
    }

    void UnPinParticle(int row, int col)
    {
        getParticle(row, col)->isFixed = false;
    }

    void dropCloth()
    {
        for (int i = 0; i < numCols; i++) {
            if (particles[i]->isFixed) UnPinParticle(0, i);
        }
    }

    void init()
    {
        /** Add particles **/
        for (int col = 0; col < numCols; col++) {
            for (int row = 0; row < numRows; row++) {
                /** Create particle by position **/
                Particle* p = new Particle(vec3((double)row / particleDensity, -((double)col / particleDensity), 0));
                /** Set texture coordinates **/
                p->texCoord.x = (double)row / (numRows - 1);
                p->texCoord.y = (double)col / (1 - numCols);
                /** Add particle to cloth **/
                particles.push_back(p);
            }
        }
        /** Add springs **/ 
        // three types of springs: https://i.imgur.com/doJrgf7.png
        for (int col = 0; col < numCols; col++) {
            for (int row = 0; row < numRows; row++) {
                /** Structural springs **/
                if (col < numCols - 1) springs.push_back(new Spring(getParticle(row, col), getParticle(row, col + 1), structuralStiffness, structuralDamping));
                if (row < numRows - 1) springs.push_back(new Spring(getParticle(row, col), getParticle(row + 1, col), structuralStiffness, structuralDamping));
                /** Shear springs **/
                if (col < numCols - 1 && row < numRows - 1) {
                    springs.push_back(new Spring(getParticle(row, col), getParticle(row + 1, col + 1), shearStiffness, shearDamping));
                    springs.push_back(new Spring(getParticle(row, col + 1), getParticle(row + 1, col), shearStiffness, shearDamping));
                }
                /** Bending springs **/
                if (col < numCols - 2) springs.push_back(new Spring(getParticle(row, col), getParticle(row, col + 2), bendingStiffness, bendingDamping));
                if (row < numRows - 2) springs.push_back(new Spring(getParticle(row, col), getParticle(row + 2, col), bendingStiffness, bendingDamping));
            }
        }
        /** Set fixed particles according to pin mode **/
        updatePinMode();
        /** Add triangles **/
        for (int col = 0; col < numCols - 1; col++) {
            for (int row = 0; row < numRows - 1; row++) {
                // Left upper triangle
                trianglePs.push_back(getParticle(row, col + 1));
                trianglePs.push_back(getParticle(row, col));
                trianglePs.push_back(getParticle(row + 1, col));
                // Right bottom triangle
                trianglePs.push_back(getParticle(row + 1, col + 1));
                trianglePs.push_back(getParticle(row, col + 1));
                trianglePs.push_back(getParticle(row + 1, col));
            }
        }
    }

    void updateNormal()
    {
        /** Reset particle's normal **/
        for (int i = 0; i < particles.size(); i++) {
            particles[i]->normal.setAsZero();
        }
        /** Compute normal of each triangle **/
        for (int i = 0; i < trianglePs.size() / 3; i++) { // 3 particles in each triangle
            Particle* p1 = trianglePs[3 * i];
            Particle* p2 = trianglePs[3 * i + 1];
            Particle* p3 = trianglePs[3 * i + 2];
            // Triangle normal
            vec3 trgNormal = vec3::cross(p2->position - p1->position, p3->position - p1->position);
            // Add triangle normal to particles it contains
            p1->normal += trgNormal;
            p2->normal += trgNormal;
            p3->normal += trgNormal;
        }
        /** Normalize particle's normal **/
        for (int i = 0; i < particles.size(); i++) {
            particles[i]->normal.normalize();
        }
    }

    void updatePinMode()
    {
        /** Set fixed particles according to pin mode **/
        if (pinMode == "Pin Upper Corner") {
            for (int i = 0; i < numCols; i++) {
                UnPinParticle(0, i);
            }
            PinParticle(0, 0, { 1.0,0.0,0.0 });
            PinParticle(0, numCols - 1, { -1.0,0.0,0.0 });
        }
        else if (pinMode == "Pin Upper Edge") {
            for (int i = 0; i < numCols; i++) {
                PinParticle(0, i, { 1.0 - i * (2.0 / (numCols - 1)),0.0,0.0 });
            }
        }
    }

    void addGrabForce(vec3 grabForce)
    {
        for (int i = 0; i < particles.size(); i++)
        {
            particles[i]->applyForce(grabForce);
        }
    }

    void computeForces(vec3 gravity, double fluidDensity, double dragCoeff, vec3 vFluid)
    {
        /** Particles **/
        for (int i = 0; i < particles.size(); i++) {
            particles[i]->applyForce(gravity * particles[i]->mass);
        }
        /** Springs **/
        for (int i = 0; i < springs.size(); i++) {
            springs[i]->applyForce();
        }
        /** Aerodynamic force on triangles (then averagely to its particles) **/
        for (int i = 0; i < trianglePs.size() / 3; i++) {
            Particle* p1 = trianglePs[3 * i];
            Particle* p2 = trianglePs[3 * i + 1];
            Particle* p3 = trianglePs[3 * i + 2];

            vec3 vSurface = (p1->velocity + p2->velocity + p3->velocity) / 3.0;
            vec3 relativeV = vSurface - vFluid;
            // Triangle normal
            vec3 trgNormal = vec3::cross(p2->position - p1->position, p3->position - p1->position);
            // Triangle area
            double area0 = 0.5 * (vec3::cross(p2->position - p1->position, p3->position - p1->position)).length();
            // Cross-sectional area
            double area = area0 * vec3::dot(vec3::getNormalized(relativeV), trgNormal);
            // Aerodynamic force on triangle surface
            vec3 force = -0.5 * fluidDensity * relativeV.length() * relativeV.length() * dragCoeff * area * trgNormal;
            
            // Apply aerodynamic force averagely to particles of triangle
            p1->applyForce(force / 3.0);
            p2->applyForce(force / 3.0);
            p3->applyForce(force / 3.0);
        }
    }

    void integrateMotion(double deltaT)
    {
        for (int i = 0; i < particles.size(); i++) particles[i]->integrateMotion(deltaT);
    }

    void collisionResponse(Ground* ground, Sphere* sphere)
    {
        for (int i = 0; i < particles.size(); i++)
        {
            /** Ground collision **/
            vec3 pWorldPos = clothPos + particles[i]->position;
            if (pWorldPos.y < ground->position.y) {
                particles[i]->position.y = ground->position.y - clothPos.y + 0.01;
                particles[i]->velocity *= ground->restitution;
            }

            /** Sphere collision **/
            vec3 center2Particel = pWorldPos - sphere->center;
            double dist = center2Particel.length();
            double collisionDist = sphere->radius * 1.05; // 5% redundancy
            if (dist < collisionDist) {
                center2Particel.normalize();
                particles[i]->position = sphere->center + collisionDist * center2Particel - clothPos;
                particles[i]->velocity *= sphere->restitution;
            }
        }
    }

    void transform(const char* trsfInstruction, float clothMoveSpeed)
    {
        glm::mat4 T;
        if (trsfInstruction == "Up") { // +y
            T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, clothMoveSpeed, 0.0f));
        }
        if (trsfInstruction == "Down") { // -y
            T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -clothMoveSpeed, 0.0f));
        }
        if (trsfInstruction == "Left") { // -x
            T = glm::translate(glm::mat4(1.0f), glm::vec3(-clothMoveSpeed, 0.0f, 0.0f));
        }
        if (trsfInstruction == "Right") { // +x
            T = glm::translate(glm::mat4(1.0f), glm::vec3(clothMoveSpeed, 0.0f, 0.0f));
        }
        if (trsfInstruction == "Inward") { // -z
            T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -clothMoveSpeed));
        }
        if (trsfInstruction == "Outward") { // +z
            T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, clothMoveSpeed));
        }
        if (trsfInstruction == "Rotate Clockwise") {
            T = glm::rotate(glm::mat4(1.0f), -2 * clothMoveSpeed, glm::vec3(0, 1.0f, 0));
        }
        if (trsfInstruction == "Rotate Counterclockwise") {
            T = glm::rotate(glm::mat4(1.0f), 2 * clothMoveSpeed, glm::vec3(0, 1.0f, 0));
        }
        glm::vec4 pos;
        for (int i = 0; i < particles.size(); i++) {
            pos = { particles[i]->position.x,  particles[i]->position.y,  particles[i]->position.z, 1.0f };
            pos = T * pos;
            particles[i]->position = vec3(pos.x, pos.y, pos.z);
        }
    }

    void reset()
    {
        for (int i = 0; i < particles.size(); i++) { particles[i]->reset(); }
        updatePinMode();
    }
};
