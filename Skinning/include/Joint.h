#pragma once
#include "core.h"
#include "DOF.h"
#include "Tokenizer.h"
#include "Cube.h"
#include <vector>

class Joint {
public:
	glm::vec3 offset;
	glm::vec3 boxmin;
	glm::vec3 boxmax;
	glm::vec3 pose; // default pose for DOFs
	glm::mat4 L;
	glm::mat4 W;
	Cube* cube;
	std::vector<DOF*> JointDOF;
	std::vector<Joint*> children;
	glm::mat4 inverseB;  // inverse of binding matrix for each joint
	char JointName[256];

	Joint();
	~Joint();

	bool Load(Tokenizer* tknizer);
	void Update(glm::mat4 parentW);
	void ResetAll();
	void AddChild(Joint* newChild);
	void Draw(const glm::mat4& viewProjMtx, GLuint shader);
	// Get subsequent joints including the current one and put them in a vector
	// Pass in joint vector by reference, should be std::vector<Joint*>*
	void BuildJointVector(std::vector<Joint*>* joints_ref);
};
