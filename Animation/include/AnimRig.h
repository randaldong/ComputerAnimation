#pragma once

#include "Skeleton.h"
#include "Skin.h"

class AnimRig
{
public:
	Skeleton* skeleton;
	Skin* skin;

	AnimRig();
	~AnimRig();

	bool Load(const char* skelfile, const char* skinfile);
	void Update(glm::mat4 parentW);
	// default: draw attached skin without skel
	void Draw(const glm::mat4& viewProjMtx, GLuint shader);
};