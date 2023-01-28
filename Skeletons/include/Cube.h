#pragma once

#include <vector>

#include "core.h"

class Cube {
private:
    GLuint VAO;
    GLuint VBO_positions, VBO_normals, EBO;

    glm::mat4 modelMtx; // model transformation matrix
    glm::vec3 color;

    // Cube Information
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

public:
    Cube();
    ~Cube();

    void drawCube(const glm::mat4& modelMtx, const glm::mat4& viewProjMtx, GLuint shader);
    void updateCube();
    void spinCube(float deg);
    void buildCube(glm::vec3 cubeMin = glm::vec3(-1, -1, -1), glm::vec3 cubeMax = glm::vec3(1, 1, 1));
};