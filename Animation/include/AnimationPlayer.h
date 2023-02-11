#pragma once
#include "core.h"
#include "AnimationClip.h"
#include "AnimRig.h"

class AnimationPlayer {
public:
	AnimationClip* clip;
	// Need a skeleton to map the pose vector to a specific rig
	AnimRig* rig;
	float curTime, tStart, tEnd;
	float deltaT = 0.01f; // 60fps, slow-motion video
	float playSpeed = 1.0f; // playback speed
	std::vector<float> poses;
	glm::mat4 rootTranslation;

	AnimationPlayer(AnimationClip* Clip, AnimRig* Rig);
	~AnimationPlayer();
	void Update(); // evaluates current poses, set these poses, increments current time
};