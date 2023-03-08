#pragma once
#include "Cloth.hpp"
#include "Program.hpp"
#include "stb_image.h"

//------------------------------------------------------Light------------------------------------------------------
struct PointLight
{
    glm::vec3 pos = glm::vec3(3.0f, 7.0f, 3.0f);
    glm::vec3 color = glm::vec3(0.7f, 0.7f, 1.0f);
};
PointLight pointLight;

//------------------------------------------------------Camera------------------------------------------------------
class Camera
{
public:
    // Perspective controls
    float FOV = 45.0f;       // Field of View Angle (degrees)
    float Aspect = 1.33f;    // Aspect Ratio
    float NearClip = 0.1f;  // Near clipping plane distance
    float FarClip = 100.0f;   // Far clipping plane distance

    float moveSpeed = 0.05f;  // position change per frame
    glm::vec3 cameraPos;
    glm::vec3 lookAtDir;
    glm::vec3 up;

    glm::mat4 viewMtx;
    glm::mat4 projectMtx;

    Camera() { Reset(); }

    void Update()
    {
        viewMtx = glm::lookAt(cameraPos, cameraPos + lookAtDir, up);
        projectMtx = glm::perspective(glm::radians(FOV), Aspect, NearClip, FarClip);
    }

    void Reset()
    {
        cameraPos = { 0.0f, 5.0f, 12.0f };
        lookAtDir = { 0.0f, 0.0f, -3.0f };
        up = { 0.0f, 1.0f, 0.0f };
        viewMtx = glm::mat4(1.0f);
        projectMtx = glm::perspective(glm::radians(FOV), Aspect, NearClip, FarClip);
    }
};
Camera camera;

//------------------------------------------------------ClothRenderer------------------------------------------------------
class ClothRenderer
{
public:
    Cloth* cloth;
    int numParticles; // # particles in all triangles

    GLuint programID;
    // VAO
    GLuint vaoID;
    // attribute pointers of VOA
    GLuint aPtrPos;
    GLuint aPtrNor;
    GLuint aPtrTex;
    // VBO
    GLuint vboIDs[3];
    glm::vec3* vboPos; // Position
    glm::vec3* vboNor; // Normal
    glm::vec2* vboTex; // Texture
    // Texture
    GLuint texID;

    ClothRenderer(Cloth* clothArg, const char* clothTexFilename)
    {
        cloth = clothArg;
        numParticles = cloth->trianglePs.size();
        if (numParticles <= 0) std::cout << "ERROR::ClothRenderer : No particle exists." << std::endl;

        vboPos = new glm::vec3[numParticles];
        vboNor = new glm::vec3[numParticles];
        vboTex = new glm::vec2[numParticles];
        for (int i = 0; i < numParticles; i++) {
            Particle* p = cloth->trianglePs[i];
            vboPos[i] = glm::vec3(p->position.x, p->position.y, p->position.z);
            vboNor[i] = glm::vec3(p->normal.x, p->normal.y, p->normal.z);
            vboTex[i] = glm::vec2(p->texCoord.x, p->texCoord.y);
        }

        /** Build shader program **/
        Program program("Shaders/ClothVS.glsl", "Shaders/ClothFS.glsl");
        programID = program.ID;
        if (programID) std::cout << "Cloth Program ID: " << programID << std::endl;

        // Generate ID of VAO and VBOs
        glGenVertexArrays(1, &vaoID);
        glGenBuffers(3, vboIDs);

        // Attribute pointers of VAO
        aPtrPos = 0;
        aPtrNor = 1;
        aPtrTex = 2;
        // Bind VAO
        glBindVertexArray(vaoID);

        // Position buffer
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
        glVertexAttribPointer(aPtrPos, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBufferData(GL_ARRAY_BUFFER, numParticles * sizeof(glm::vec3), vboPos, GL_DYNAMIC_DRAW);
        // Normal buffer
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
        glVertexAttribPointer(aPtrNor, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBufferData(GL_ARRAY_BUFFER, numParticles * sizeof(glm::vec3), vboNor, GL_DYNAMIC_DRAW);
        // Texture buffer
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
        glVertexAttribPointer(aPtrTex, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBufferData(GL_ARRAY_BUFFER, numParticles * sizeof(glm::vec2), vboTex, GL_DYNAMIC_DRAW);
        // Enable it's attribute pointers since they were set well
        glEnableVertexAttribArray(aPtrPos);
        glEnableVertexAttribArray(aPtrNor);
        glEnableVertexAttribArray(aPtrTex);

        /** Load texture **/
        // Assign texture ID and gengeration
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        // Set the texture wrapping parameters (for 2D tex: S, T)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // Set texture filtering parameters (Minify, Magnify)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        /** Load image and configure texture **/
        stbi_set_flip_vertically_on_load(true);
        int texW, texH, colorChannels; // After loading the image, stb_image will fill them
        unsigned char* data = stbi_load(clothTexFilename, &texW, &texH, &colorChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texW, texH, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            // Automatically generate all the required mipmaps for the currently bound texture.
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cout << "Failed to load texture for Cloth" << std::endl;
        }
        // Always free image memory
        stbi_image_free(data);

        /** Set uniform **/
        glUseProgram(programID); // Active shader before set uniform
        // Set texture sampler
        glUniform1i(glGetUniformLocation(programID, "uniTexSmplr"), 0);

        /** Model Matrix : Put cloth into the world **/
        glm::mat4 uniModelMatrix = glm::mat4(1.0f);
        uniModelMatrix = glm::translate(uniModelMatrix, glm::vec3(cloth->clothPos.x, cloth->clothPos.y, cloth->clothPos.z));
        glUniformMatrix4fv(glGetUniformLocation(programID, "uniModelMtx"), 1, GL_FALSE, &uniModelMatrix[0][0]);

        // Cleanup
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbined VBO
        glBindVertexArray(0); // Unbined VAO
    }

    ~ClothRenderer()
    {
        delete[] vboPos;
        delete[] vboNor;
        delete[] vboTex;

        if (vaoID)
        {
            glDeleteVertexArrays(1, &vaoID);
            glDeleteBuffers(3, vboIDs);
            vaoID = 0;
        }
        if (programID)
        {
            glDeleteProgram(programID);
            programID = 0;
        }
    }

    void Update()
    {
        for (int i = 0; i < numParticles; i++) {
            Particle* p = cloth->trianglePs[i];
            vboPos[i] = glm::vec3(p->position.x, p->position.y, p->position.z);
            vboNor[i] = glm::vec3(p->normal.x, p->normal.y, p->normal.z);
        }

        glUseProgram(programID);

        glBindVertexArray(vaoID);

        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, numParticles * sizeof(glm::vec3), vboPos);
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, numParticles * sizeof(glm::vec3), vboNor);
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, numParticles * sizeof(glm::vec2), vboTex);

        /** Bind texture **/
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID);

        /** Update camera **/
        camera.Update();
        glUniformMatrix4fv(glGetUniformLocation(programID, "uniViewMtx"), 1, GL_FALSE, &camera.viewMtx[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(programID, "uniProjMtx"), 1, GL_FALSE, &camera.projectMtx[0][0]);
        /** Update light **/
        glUniform3fv(glGetUniformLocation(programID, "uniLightPos"), 1, &(pointLight.pos[0]));
        glUniform3fv(glGetUniformLocation(programID, "uniLightColor"), 1, &(pointLight.color[0]));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        /** Draw **/
        switch (cloth->drawMode) {
        case Cloth::DRAW_PARTICLES:
            glDrawArrays(GL_POINTS, 0, numParticles);
            break;
        case Cloth::DRAW_SPRINGS:
            glDrawArrays(GL_LINES, 0, numParticles);
            break;
        case Cloth::DRAW_CLOTH:
            glDrawArrays(GL_TRIANGLES, 0, numParticles);
            break;
        }

        glDisable(GL_BLEND);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);
    }
};

//------------------------------------------------------SpringRenderer------------------------------------------------------
class SpringRenderer
{
public:
    std::vector<Spring*> springs;
    int numSprings;

    GLuint programID;
    GLuint vaoID;
    GLuint vboIDs[2];
    GLint aPtrPos;
    GLint aPtrNor;
    glm::vec3* vboPos; // Position
    glm::vec3* vboNor; // Normal
    glm::vec4 uniSpringColor;

    SpringRenderer(std::vector<Spring*> springsArg, glm::vec4 springColorArg, glm::vec3 modelMtxArg)
    {
        springs = springsArg;
        numSprings = springs.size();
        if (numSprings <= 0) std::cout << "ERROR::SpringRenderer : No spring exists." << std::endl;
        uniSpringColor = springColorArg;

        vboPos = new glm::vec3[numSprings * 2];
        vboNor = new glm::vec3[numSprings * 2];
        for (int i = 0; i < numSprings; i++) {
            Particle* p1 = springs[i]->p1;
            Particle* p2 = springs[i]->p2;
            vboPos[i * 2] = glm::vec3(p1->position.x, p1->position.y, p1->position.z);
            vboPos[i * 2 + 1] = glm::vec3(p2->position.x, p2->position.y, p2->position.z);
            vboNor[i * 2] = glm::vec3(p1->normal.x, p1->normal.y, p1->normal.z);
            vboNor[i * 2 + 1] = glm::vec3(p2->normal.x, p2->normal.y, p2->normal.z);
        }

        /** Build shader program **/
        Program program("Shaders/SpringVS.glsl", "Shaders/SpringFS.glsl");
        programID = program.ID;
        if (programID) std::cout << "Spring Program ID: " << programID << std::endl;

        // Generate ID of VAO and VBOs
        glGenVertexArrays(1, &vaoID);
        glGenBuffers(2, vboIDs);

        // Attribute pointers of VAO
        aPtrPos = 0;
        aPtrNor = 1;
        // Bind VAO
        glBindVertexArray(vaoID);

        // Position buffer
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
        glVertexAttribPointer(aPtrPos, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBufferData(GL_ARRAY_BUFFER, numSprings * 2 * sizeof(glm::vec3), vboPos, GL_DYNAMIC_DRAW);
        // Normal buffer
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
        glVertexAttribPointer(aPtrNor, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBufferData(GL_ARRAY_BUFFER, numSprings * 2 * sizeof(glm::vec3), vboNor, GL_DYNAMIC_DRAW);

        // Enable it's attribute pointers since they were set well
        glEnableVertexAttribArray(aPtrPos);
        glEnableVertexAttribArray(aPtrNor);

        /** Set uniform **/
        glUseProgram(programID); // Active shader before set uniform
        // Set color
        glUniform4fv(glGetUniformLocation(programID, "uniSpringColor"), 1, &uniSpringColor[0]);

        /** Model Matrix : Put rigid into the world **/
        glm::mat4 uniModelMatrix = glm::mat4(1.0f);
        uniModelMatrix = glm::translate(uniModelMatrix, modelMtxArg);
        glUniformMatrix4fv(glGetUniformLocation(programID, "uniModelMtx"), 1, GL_FALSE, &uniModelMatrix[0][0]);

        // Cleanup
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbined VBO
        glBindVertexArray(0); // Unbined VAO
    }

    ~SpringRenderer()
    {
        delete[] vboPos;
        delete[] vboNor;

        if (vaoID)
        {
            glDeleteVertexArrays(1, &vaoID);
            glDeleteBuffers(2, vboIDs);
            vaoID = 0;
        }
        if (programID)
        {
            glDeleteProgram(programID);
            programID = 0;
        }
    }

    void Update()
    {
        for (int i = 0; i < numSprings; i++) {
            Particle* p1 = springs[i]->p1;
            Particle* p2 = springs[i]->p2;
            vboPos[i * 2] = glm::vec3(p1->position.x, p1->position.y, p1->position.z);
            vboPos[i * 2 + 1] = glm::vec3(p2->position.x, p2->position.y, p2->position.z);
            vboNor[i * 2] = glm::vec3(p1->normal.x, p1->normal.y, p1->normal.z);
            vboNor[i * 2 + 1] = glm::vec3(p2->normal.x, p2->normal.y, p2->normal.z);
        }

        glUseProgram(programID);

        glBindVertexArray(vaoID);

        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, numSprings * 2 * sizeof(glm::vec3), vboPos);
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, numSprings * 2 * sizeof(glm::vec3), vboNor);

       /** Update camera **/
        camera.Update();
        glUniformMatrix4fv(glGetUniformLocation(programID, "uniViewMtx"), 1, GL_FALSE, &camera.viewMtx[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(programID, "uniProjMtx"), 1, GL_FALSE, &camera.projectMtx[0][0]);
        /** Update light **/
        glUniform3fv(glGetUniformLocation(programID, "uniLightPos"), 1, &(pointLight.pos[0]));
        glUniform3fv(glGetUniformLocation(programID, "uniLightColor"), 1, &(pointLight.color[0]));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        /** Draw **/
        glDrawArrays(GL_LINES, 0, numSprings * 2);

        glDisable(GL_BLEND);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);
    }
};

//------------------------------------------------------ClothSpringRenderer------------------------------------------------------
class ClothSpringRenderer
{
public:
    Cloth* cloth;
    SpringRenderer* springRenderer;
    glm::vec4 springColor;

    ClothSpringRenderer(Cloth* clothArg)
    {
        cloth = clothArg;
        springColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
        springRenderer = new SpringRenderer(cloth->springs, springColor, glm::vec3(cloth->clothPos.x, cloth->clothPos.y, cloth->clothPos.z));
    }

    void Update() { springRenderer->Update(); }
};

//------------------------------------------------------RigidRenderer------------------------------------------------------
class RigidRenderer
{
public:
    std::vector<Vertex*> triangleVs;
    int numVertices;
    bool isUseColor; // light use it's own color
    bool isTransparent;

    GLuint programID;
    // VAO
    GLuint vaoID;
    // attribute pointers of VOA
    GLint aPtrPos;
    GLint aPtrNor;
    GLint aPtrTex;
    // VBO
    GLuint vboIDs[3];
    glm::vec3* vboPos;
    glm::vec3* vboNor;
    glm::vec2* vboTex;
    // Texture    
    GLuint texID;

    RigidRenderer(std::vector<Vertex*> triangleVsArg, const char* texFilename, glm::vec3 modelMtxArg, bool isUseColorArg, bool isTransparentArg)
    {
        triangleVs = triangleVsArg;
        numVertices = triangleVs.size();
        isUseColor = isUseColorArg;
        isTransparent = isTransparentArg;
        if (numVertices <= 0) std::cout << "ERROR::RigidRenderer : No vertex exists." << std::endl;

        vboPos = new glm::vec3[numVertices];
        vboNor = new glm::vec3[numVertices];
        vboTex = new glm::vec2[numVertices];
        for (int i = 0; i < numVertices; i++) {
            Vertex* v = triangleVs[i];
            vboPos[i] = glm::vec3(v->position.x, v->position.y, v->position.z);
            vboNor[i] = glm::vec3(v->normal.x, v->normal.y, v->normal.z);
            vboTex[i] = glm::vec2(v->texCoord.x, v->texCoord.y);
        }

        /** Build render program **/
        Program program("Shaders/RigidVS.glsl", "Shaders/RigidFS.glsl");
        programID = program.ID;
        if (programID) std::cout << "Rigid Program ID: " << programID << std::endl;

        // Generate ID of VAO and VBOs
        glGenVertexArrays(1, &vaoID);
        glGenBuffers(3, vboIDs);

        // Attribute pointers of VAO
        aPtrPos = 0;
        aPtrNor = 1;
        aPtrTex = 2;
        // Bind VAO
        glBindVertexArray(vaoID);

        // Position buffer
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
        glVertexAttribPointer(aPtrPos, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(glm::vec3), vboPos, GL_DYNAMIC_DRAW);
        // Normal buffer
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
        glVertexAttribPointer(aPtrNor, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(glm::vec3), vboNor, GL_DYNAMIC_DRAW);
        // Texture buffer
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
        glVertexAttribPointer(aPtrTex, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(glm::vec2), vboTex, GL_DYNAMIC_DRAW);

        // Enable it's attribute pointers since they were set well
        glEnableVertexAttribArray(aPtrPos);
        glEnableVertexAttribArray(aPtrNor);
        glEnableVertexAttribArray(aPtrTex);

        /** Load texture **/
        // Assign texture ID and gengeration
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        // Set the texture wrapping parameters (for 2D tex: S, T)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // Set texture filtering parameters (Minify, Magnify)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        /** Load image and configure texture **/
        stbi_set_flip_vertically_on_load(true);
        int texW, texH, colorChannels; // After loading the image, stb_image will fill them
        unsigned char* data = stbi_load(texFilename, &texW, &texH, &colorChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texW, texH, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            // Automatically generate all the required mipmaps for the currently bound texture.
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cout << "Failed to load texture for Rigid: " << texFilename << std::endl;
        }
        // Always free image memory
        stbi_image_free(data);

        /** Set uniform **/
        glUseProgram(programID); // Active shader before set uniform
        // Set texture sampler
        glUniform1i(glGetUniformLocation(programID, "uniTexSmplr"), 0);
        // Model Matrix : Put rigid into the world
        glm::mat4 uniModelMatrix = glm::mat4(1.0f);
        uniModelMatrix = glm::translate(uniModelMatrix, modelMtxArg);
        glUniformMatrix4fv(glGetUniformLocation(programID, "uniModelMtx"), 1, GL_FALSE, &uniModelMatrix[0][0]);
        // bool uniforms
        glUniform1i(glGetUniformLocation(programID, "uniIsUseColor"), isUseColor);
        glUniform1i(glGetUniformLocation(programID, "uniIsTransparent"), isTransparent);

        // Cleanup
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbined VBO
        glBindVertexArray(0); // Unbined VAO
    }

    ~RigidRenderer()
    {
        delete[] vboPos;
        delete[] vboNor;

        if (vaoID)
        {
            glDeleteVertexArrays(1, &vaoID);
            glDeleteBuffers(2, vboIDs);
            vaoID = 0;
        }
        if (programID)
        {
            glDeleteProgram(programID);
            programID = 0;
        }
    }

    void Update() // Rigid does not move, thus do not update vertices data
    {
        for (int i = 0; i < numVertices; i++) {
            Vertex* v = triangleVs[i];
            vboPos[i] = glm::vec3(v->position.x, v->position.y, v->position.z);
            vboNor[i] = glm::vec3(v->normal.x, v->normal.y, v->normal.z);
        }

        glUseProgram(programID);

        glBindVertexArray(vaoID);

        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * sizeof(glm::vec3), vboPos);
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * sizeof(glm::vec3), vboNor);
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * sizeof(glm::vec2), vboTex);

        /** Bind texture **/
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID);

        /** Update camera **/
        camera.Update();
        glUniformMatrix4fv(glGetUniformLocation(programID, "uniViewMtx"), 1, GL_FALSE, &camera.viewMtx[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(programID, "uniProjMtx"), 1, GL_FALSE, &camera.projectMtx[0][0]);
        /** Update light **/
        glUniform3fv(glGetUniformLocation(programID, "uniLightPos"), 1, &(pointLight.pos[0]));
        glUniform3fv(glGetUniformLocation(programID, "uniLightColor"), 1, &(pointLight.color[0]));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        /** Draw **/
        glDrawArrays(GL_TRIANGLES, 0, numVertices);

        glDisable(GL_BLEND);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);
    }
};

class GroundRenderer
{
public:
    Ground* ground;
    RigidRenderer* rigidRenderer;
    bool isUseColor = false;
    bool isTransparent = false;
    GroundRenderer(Ground* groundArg, const char* groundTexFilename)
    {
        ground = groundArg;
        rigidRenderer = new RigidRenderer(ground->triangleVs, groundTexFilename, glm::vec3(ground->position.x, ground->position.y, ground->position.z), isUseColor, isTransparent);
    }

    void Update() { rigidRenderer->Update(); }
};

struct SphereRenderer
{
    Sphere* sphere;
    RigidRenderer* rigidRenderer;
    bool isUseColor;
    bool isTransparent;

    SphereRenderer(Sphere* sphereArg, const char* sphereTexFilename, bool isUseColorArg, bool isTransparentArg)
    {
        sphere = sphereArg;
        isUseColor = isUseColorArg;
        isTransparent = isTransparentArg;
        rigidRenderer = new RigidRenderer(sphere->triangleVs, sphereTexFilename, glm::vec3(sphere->center.x, sphere->center.y, sphere->center.z), isUseColor, isTransparent);
    }

    void Update() { rigidRenderer->Update(); }
};
