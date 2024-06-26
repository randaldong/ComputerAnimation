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
	// define joints that influence the current vertex; 
	// a vertex can be attached to multiple joints 
	std::vector<Joint*> joints; 

	Vertex();
	~Vertex();

	void setPositions(float px, float py, float pz);
	void setNormals(float nx, float ny, float nz);
};