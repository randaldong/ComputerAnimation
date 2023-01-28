#pragma once
#include "Joint.h"
#include "Core.h"
#include "Tokenizer.h"

class Skeleton {
public:
	Joint* root;

	Skeleton();
	~Skeleton();

	bool Load(const char* filename = "assets/test.skel");
	void Update(glm::mat4 parentW);
	void Draw(const glm::mat4& viewProjMtx, GLuint shader);
};