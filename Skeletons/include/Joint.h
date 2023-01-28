#pragma once
#include "core.h"
#include "DOF.h"
#include "Tokenizer.h"
#include "Cube.h"
#include <vector>

class Joint {
private:
	glm::vec3 offset;
	glm::vec3 boxmin;
	glm::vec3 boxmax;
	glm::mat4 L;
	glm::mat4 W;
	Cube* cube;
	std::vector<DOF*> JointDOF;
	std::vector<Joint*> children;

public:
	Joint();
	~Joint();
	bool Load(Tokenizer* tknizer);
	void Update(glm::mat4 parentW);
	void AddChild(Joint* newChild);
	void Draw(const glm::mat4& viewProjMtx, GLuint shader);
};
