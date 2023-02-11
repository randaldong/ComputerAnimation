#include "AnimRig.h"
#include <iostream>

AnimRig::AnimRig()
{
	skeleton = new Skeleton();
	skin = new Skin(skeleton);
}

AnimRig::~AnimRig()
{
	delete skeleton;
	delete skin;
}

bool AnimRig::Load(const char* skelfile, const char* skinfile)
{
	bool isLoadSkel = skeleton->Load(skelfile);
	bool isLoadSkin = skin->Load(skinfile);
	if (isLoadSkel && isLoadSkin) {
		std::cout << "Successfully Loaded Animation Rig!" << std::endl;
		return true;
	}
	else if (!isLoadSkel) {
		std::cout << "Failed To Load Skeleton for Animation Rig!" << std::endl;
		return false;
	}
	else if (!isLoadSkin) {
		std::cout << "Failed To Load Skin for Animation Rig!" << std::endl;
		return false;
	}
}

void AnimRig::Update(glm::mat4 parentW)
{
	skeleton->Update(parentW);
	skin->Update();
}

void AnimRig::Draw(const glm::mat4& viewProjMtx, GLuint shader)
{
	// default: only draw rig with attached skin, isDrawOriginalSkin = false
	skin->Draw(false, viewProjMtx, shader);
}