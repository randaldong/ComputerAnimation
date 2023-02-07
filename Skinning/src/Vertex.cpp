#include "Vertex.h"

Vertex::Vertex()
{

}

Vertex::~Vertex()
{

}

void Vertex::setPositions(float x, float y, float z)
{
	position = glm::vec3(x, y, z);
}

void Vertex::setNormals(float x, float y, float z)
{
	normal = glm::vec3(x, y, z);
}