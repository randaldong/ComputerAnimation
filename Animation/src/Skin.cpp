#include "Skin.h"
#include "glm/gtx/string_cast.hpp"
#include <iostream>

Skin::Skin(Skeleton* skel)
{
    skeleton = skel;
    // Generate a vertex array (VAO), two vertex buffer objects (VBO), and EBO.
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO_positions);
    glGenBuffers(1, &VBO_normals);
    glGenBuffers(1, &EBO);
}

Skin::~Skin()
{
    // Delete the VBOs and the VAO.
    glDeleteBuffers(1, &VBO_positions);
    glDeleteBuffers(1, &VBO_normals);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void Skin::BindBuffer()
{
    // Bind to the VAO.
    glBindVertexArray(VAO);

    // Bind to the first VBO - positions of vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * shaderPositions.size(), shaderPositions.data(), GL_STATIC_DRAW);
    GLuint posLoc = 0;
    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // Bind to the second VBO - normals of vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * shaderNormals.size(), shaderNormals.data(), GL_STATIC_DRAW);
    GLuint normLoc = 1;
    glEnableVertexAttribArray(normLoc);
    glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // Bind the EBO to the bound VAO and send the data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * shaderIndices.size(), shaderIndices.data(), GL_STATIC_DRAW);

    // Unbind the VBOs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

bool Skin::Load(const char* filename)
{
    Tokenizer* tknizer = new Tokenizer();
    tknizer->Open(filename);

    tknizer->FindToken("positions");
    vertexNum = tknizer->GetFloat();

    // Set positions
    tknizer->FindToken("{");
    float px, py, pz;
    for (int i = 0; i < vertexNum; i++) {
        Vertex* vertex = new Vertex();
        px = tknizer->GetFloat();
        py = tknizer->GetFloat();
        pz = tknizer->GetFloat();
        vertex->setPositions(px, py, pz);
        vertices.push_back(vertex);
        bindingPositions.push_back({ px, py, pz });
        shaderPositions.push_back({ px, py, pz });
    }

    // Set normals
    tknizer->FindToken("{");
    float nx, ny, nz;
    for (int i = 0; i < vertexNum; i++) {
        nx = tknizer->GetFloat();
        ny = tknizer->GetFloat();
        nz = tknizer->GetFloat();
        vertices[i]->setNormals(nx, ny, nz);
        bindingNormals.push_back({ nx, ny, nz });
        shaderNormals.push_back({ nx, ny, nz });
    }

    // Set weights
    tknizer->FindToken("{");
    int attachmentNum, JointID;
    float weight;
    for (int i = 0; i < vertexNum; i++) {
        attachmentNum = tknizer->GetFloat();
        for (int j = 0; j < attachmentNum; j++) {
            JointID = tknizer->GetFloat();
            weight = tknizer->GetFloat();
            vertices[i]->weights.push_back(weight);
            vertices[i]->joints.push_back(skeleton->joints[JointID]);
        }
    }

    // Set indices/triangles
    tknizer->FindToken("triangles");
    int triangleNum = tknizer->GetFloat();
    tknizer->FindToken("{");
    int v0, v1, v2;
    for (int i = 0; i < triangleNum; i++) {
        v0 = tknizer->GetFloat();
        v1 = tknizer->GetFloat();
        v2 = tknizer->GetFloat();
        shaderIndices.push_back(v0);
        shaderIndices.push_back(v1);
        shaderIndices.push_back(v2);
    }

    // Set binding matrices (one binding matrix to one joint)
    tknizer->FindToken("bindings");
    int bindingNum = tknizer->GetFloat(); // bindingNum = jointNum
    tknizer->FindToken("{");
    float ax, ay, az, bx, by, bz, cx, cy, cz, dx, dy, dz;
    for (int i = 0; i < bindingNum; i++) {
        tknizer->FindToken("{");
        ax = tknizer->GetFloat();
        ay = tknizer->GetFloat();
        az = tknizer->GetFloat();
        bx = tknizer->GetFloat();
        by = tknizer->GetFloat();
        bz = tknizer->GetFloat();
        cx = tknizer->GetFloat();
        cy = tknizer->GetFloat();
        cz = tknizer->GetFloat();
        dx = tknizer->GetFloat();
        dy = tknizer->GetFloat();
        dz = tknizer->GetFloat();
        skeleton->joints[i]->inverseB = glm::inverse(
            glm::mat4(
                ax, ay, az, 0.0f,
                bx, by, bz, 0.0f, 
                cx, cy, cz, 0.0f, 
                dx, dy, dz, 1.0f
            )
        );
    }

    tknizer->Close();
    BindBuffer();
    return true;
}

void Skin::Update()
{
    // Two loop;
    // Compute matrix for each joint of the current vertex;
    // Compute blended world space positions & normals for each vertex;
    Vertex* curV;
    glm::vec3 curPosition;
    glm::vec3 curNormal;
    glm::mat4 M;
    glm::vec4 transformedPosition;
    glm::vec4 transformedNormal;

    for (int i = 0; i < vertexNum; i++) {
        curV = vertices[i];
        curPosition = curV->position;
        curNormal = curV->normal;
        transformedPosition = glm::vec4(0.0f);
        transformedNormal = glm::vec4(0.0f);

        for (int j = 0; j < curV->joints.size(); j++) {
            M = curV->weights[j] * curV->joints[j]->W * curV->joints[j]->inverseB;
            transformedPosition += M * glm::vec4(curPosition, 1.0f);
            transformedNormal += M * glm::vec4(curNormal, 0.0f);
        }

        shaderPositions[i] = glm::vec3(transformedPosition);
        transformedNormal = glm::normalize(transformedNormal);
        shaderNormals[i] = glm::vec3(transformedNormal);
    }
}

void Skin::Draw(bool isDrawOriginalSkin, const glm::mat4& viewProjMtx, GLuint shader)
{
    glm::mat4 modelMtx = glm::mat4(1.0f);
    glm::mat4 mvpMtx = viewProjMtx * modelMtx;
    glm::vec3 ambientColor = { 0.1, 0.05, 0.8 }; // final reflected ambient color

    // actiavte the shader program
    glUseProgram(shader);

    // get the locations and send the uniforms to the shader
    glUniformMatrix4fv(glGetUniformLocation(shader, "ModelMtx"), 1, false, (float*)&modelMtx);
    glUniformMatrix4fv(glGetUniformLocation(shader, "ModelViewProjectionMtx"), 1, GL_FALSE, (float*)&mvpMtx);
    glUniform3fv(glGetUniformLocation(shader, "AmbientColor"), 1, &ambientColor[0]);
    
    // rebind and resend new data after transforming positions and normals
    // Rebind the VAO
    glBindVertexArray(VAO);

    // Update the first VBO - positions of vertices - then unbind
    glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
    if (isDrawOriginalSkin) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * bindingPositions.size(), bindingPositions.data(), GL_STATIC_DRAW);
    }
    else {
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * shaderPositions.size(), shaderPositions.data(), GL_STATIC_DRAW);

    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Update the second VBO - normals of vertices - then unbind
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
    if (isDrawOriginalSkin) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * bindingNormals.size(), bindingNormals.data(), GL_STATIC_DRAW);
    }
    else {
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * shaderNormals.size(), shaderNormals.data(), GL_STATIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // draw the points using triangles, indexed with the EBO
    glDrawElements(GL_TRIANGLES, shaderIndices.size(), GL_UNSIGNED_INT, 0);

    // Unbind the VAO and shader program
    glBindVertexArray(0);
    glUseProgram(0);
}