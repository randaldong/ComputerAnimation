#pragma once
#include "Joint.h"
#include "Core.h"
#include "Tokenizer.h"
#include <vector>

class Skeleton {
public:
	Joint* root;
	// Joints contained in the whole skeleton
	// used for linking joints with skin when setting weights
	std::vector<Joint*> joints; 

	Skeleton();
	~Skeleton();

	bool Load(const char* filename = "assets/test.skel");
	void Update(glm::mat4 parentW);
	void Draw(const glm::mat4& viewProjMtx, GLuint shader);
	void BuildJointVector();
};