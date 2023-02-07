#pragma once
#include "Core.h"
#include <vector>
#include "Joint.h"

class Vertex
{
public:
	glm::vec3 position;
	glm::vec3 normal;
	std::vector<float> weights;
	std::vector<Joint*> joints; // A vertex can be attached to multiple vertices

	Vertex();
	~Vertex();

	void setPositions(float px, float py, float pz);
	void setNormals(float nx, float ny, float nz);
};