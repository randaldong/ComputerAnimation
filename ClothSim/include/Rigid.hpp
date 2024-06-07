#pragma once
#define _USE_MATH_DEFINES // in order to use M_PI in math.h
#include <math.h>
#include "Particle.hpp"

//------------------------------------------------------Ground------------------------------------------------------
class Ground
{
public:
    vec3 position;
    int width, height;
    float restitution;

    std::vector<Vertex*> vertices;
    std::vector<Vertex*> triangleVs; // vertices that make up of rigid triangles

    Ground(vec3 pos, vec2 size, double reGround = 0.6f) 
    {
        position = pos;
        width = size.x;
        height = size.y;
        restitution = reGround;
        
        vertices.push_back(new Vertex(vec3(0.0, 0.0, 0.0)));
        vertices.push_back(new Vertex(vec3(width, 0.0, 0.0)));
        vertices.push_back(new Vertex(vec3(0.0, 0.0, -height)));
        vertices.push_back(new Vertex(vec3(width, 0.0, -height)));

        for (int i = 0; i < vertices.size(); i++) {
            vertices[i]->normal = vec3(0.0, 1.0, 0.0);
        }

        vertices[0]->texCoord = { 0.0, 0.0 };
        vertices[1]->texCoord = { 1.0, 0.0 };
        vertices[2]->texCoord = { 0.0, 1.0 };
        vertices[3]->texCoord = { 1.0, 1.0 };

        triangleVs.push_back(vertices[0]);
        triangleVs.push_back(vertices[1]);
        triangleVs.push_back(vertices[2]);
        triangleVs.push_back(vertices[1]);
        triangleVs.push_back(vertices[2]);
        triangleVs.push_back(vertices[3]);
    }
};

class Sphere
{
public:
    float radius;
    vec3 center;
    int numRows = 180;
    int numCols = 360;
    float restitution;

    std::vector<Vertex*> vertices;
    std::vector<Vertex*> triangleVs;

    Sphere(double radiusArg, vec3 centerArg, float reSphere = 0.8f)
    {
        radius = radiusArg;
        center = centerArg;
        restitution = reSphere;
        init();
    }

    Vertex* getVertex(int row, int col)
    {
        if (row < 0 || row >= numRows || col < 0 || col >= numCols) printf("Vertex Index Out of Range.\n");
        else return vertices[1 + col * numRows + row];
    }

    void computeSphereNormal()
    {
        /** Reset nodes' normal **/
        vec3 normal(0.0, 0.0, 0.0);
        for (int i = 0; i < vertices.size(); i++) {
            vertices[i]->normal = normal;
        }

        for (int i = 0; i < triangleVs.size() / 3; i++) {
            Vertex* v1 = triangleVs[i * 3];
            Vertex* v2 = triangleVs[i * 3 + 1];
            Vertex* v3 = triangleVs[i * 3 + 2];

            vec3 triangleNor = vec3::cross(v2->position - v1->position, v3->position - v1->position);
            v1->normal += triangleNor;
            v2->normal += triangleNor;
            v3->normal += triangleNor;
        }
        /** Normalize **/
        for (int i = 0; i < vertices.size(); i++) {
            vertices[i]->normal.normalize();
        }
    }

    void init() // Initialize vertices pos & tex, slice triangles
    {
        /** Compute vertex position & texCoord **/
        double unitY = radius * 2.0 / (numCols + 1); // how many lines cut this sphere
        double unitRadian = 2.0 * M_PI / numRows;

        vertices.clear();
        triangleVs.clear();
        vec3 pos(0.0, radius, 0.0);
        vertices.push_back(new Vertex(pos)); // Top vertex

        for (int col = 0; col < numCols; col++) {
            pos.y -= unitY;
            for (int row = 0; row < numRows; row++) {
                double xzLen = sqrt(radius * radius - pos.y * pos.y);
                double xRadian = row * unitRadian;  // the projection len on x-z plane

                pos.x = xzLen * sin(xRadian);
                pos.z = xzLen * cos(xRadian);
                Vertex* v = new Vertex(pos);
                // let texture center show in the front
                double xCoord = (double)(row + (numRows - 1) / 2) / (numRows - 1);
                if (xCoord > 1) xCoord -= 1;
                v->texCoord.x = xCoord;
                v->texCoord.y = -(double)col / (numCols - 1);
                vertices.push_back(v);
            }
        }
        pos = vec3(0.0, -radius, 0.0);
        vertices.push_back(new Vertex(pos)); // Bottom vertex

        /** Slice triangles **/
        // Top ring of the sphere = first column
        for (int row = 0; row < numRows; row++) {
            triangleVs.push_back(getVertex(row, 0));
            triangleVs.push_back(vertices[0]);
            triangleVs.push_back(getVertex((row + 1) % numRows, 0));
        }
        // Middle rings
        for (int col = 0; col < numCols - 1; col++) {
            for (int row = 0; row < numRows; row++) {
                triangleVs.push_back(getVertex(row, col));
                triangleVs.push_back(getVertex((row + 1) % numRows, col));
                triangleVs.push_back(getVertex(row, col + 1));

                triangleVs.push_back(getVertex((row + 1) % numRows, col + 1));
                triangleVs.push_back(getVertex(row, col + 1));
                triangleVs.push_back(getVertex((row + 1) % numRows, col));
            }
        }
        // Bottom ring of the sphere = last column
        int lastIndex = vertices.size() - 1;
        for (int row = 0; row < numRows; row++) {
            triangleVs.push_back(vertices[lastIndex]);
            triangleVs.push_back(getVertex(row, numCols - 1));
            triangleVs.push_back(getVertex((row + 1) % numRows, numCols - 1));
        }

        /** Set triangle vertices' normals **/
        computeSphereNormal();
    }

    void translate(const char* dir, float sphereMoveSpeed)
    {
        glm::mat4 T;
        if (dir == "Up") { // +y
            T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sphereMoveSpeed, 0.0f));
        }
        if (dir == "Down") { // -y
            T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -sphereMoveSpeed, 0.0f));
        }
        if (dir == "Left") { // -x
            T = glm::translate(glm::mat4(1.0f), glm::vec3(-sphereMoveSpeed, 0.0f, 0.0f));
        }
        if (dir == "Right") { // +x
            T = glm::translate(glm::mat4(1.0f), glm::vec3(sphereMoveSpeed, 0.0f, 0.0f));
        }
        if (dir == "Inward") { // -z
            T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -sphereMoveSpeed));
        }
        if (dir == "Outward") { // +z
            T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, sphereMoveSpeed));
        }
        glm::vec4 pos;
        for (int i = 0; i < vertices.size(); i++) {
            pos = { vertices[i]->position.x,  vertices[i]->position.y,  vertices[i]->position.z, 1.0f };
            pos = T * pos;
            vertices[i]->position = vec3(pos.x, pos.y, pos.z);
        }
    }
};

