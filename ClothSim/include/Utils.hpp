#pragma once
#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <vector>
#include <iostream>

//------------------------------------------------------vec2------------------------------------------------------
// write the following vector tool below so that double can be handled easily
// since glm::vec2 & glm::vec3 deals with float mostly

struct vec2
{
    double x;
    double y;

    vec2(void)
    {
        x = 0.0;
        y = 0.0;
    }

    vec2(double x0, double y0)
    {
        x = x0;
        y = y0;
    }

    vec2 operator+(vec2 v)
    {
        return vec2(x + v.x, y + v.y);
    }

    vec2 operator-(vec2 v)
    {
        return vec2(x - v.x, y - v.y);
    }

    void operator+=(vec2 v)
    {
        x += v.x;
        y += v.y;
    }

    void operator-=(vec2 v)
    {
        x -= v.x;
        y -= v.y;
    }
};

//------------------------------------------------------vec3------------------------------------------------------
struct vec3
{
    double x;
    double y;
    double z;

    vec3(void)
    {
        x = 0.0;
        y = 0.0;
        z = 0.0;
    }

    vec3(double x0, double y0, double z0)
    {
        x = x0;
        y = y0;
        z = z0;
    }

    static vec3 cross(vec3 v1, vec3 v2)
    {
        return vec3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
    }

    static vec3 getNormalized(vec3 v)
    {
        double w = v.length();
        return vec3(v.x / w, v.y / w, v.z / w);
    }

    static double dot(vec3 v1, vec3 v2)
    {
        return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
    }

    static double dist(vec3 v1, vec3 v2)
    {
        return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
    }

    vec3 negative()
    {
        return vec3(-x, -y, -z);
    }

    vec3 operator+(vec3 v)
    {
        return vec3(x + v.x, y + v.y, z + v.z);
    }

    vec3 operator-(vec3 v)
    {
        return vec3(x - v.x, y - v.y, z - v.z);
    }

    void operator+=(vec3 v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    void operator-=(vec3 v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }

    vec3 operator*(double n) // vec3 * double, can be a member function
    {
        return vec3(x * n, y * n, z * n);
    }

    vec3 operator/(double n)
    {
        return vec3(x / n, y / n, z / n);
    }

    void operator*=(double n)
    {
        x *= n;
        y *= n;
        z *= n;
    }

    bool operator==(vec3& v)
    {
        return x == v.x && y == v.y && z == v.z;
    }

    bool operator!=(vec3& v)
    {
        return x != v.x || y != v.y || z != v.z;
    }

    double length()
    {
        return sqrt(x * x + y * y + z * z);
    }

    void normalize()
    {
        double w = length();
        if (w < 0.00001) return;

        x /= w;
        y /= w;
        z /= w;
    }

    void setAsZero() {
        x = 0.0;
        y = 0.0;
        z = 0.0;
    }
};

vec3 operator*(double n, vec3& v) // double * vec3
{
    return v * n;
}